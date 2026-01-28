/**
 * STM32 Serving Robot Web Controller v1.3
 * 
 * 수정사항:
 * - ZONE 파싱 추가 (SEQ → ZONE)
 * - LOAD/UNLOAD 텍스트 파싱 추가
 */

const express = require('express');
const { SerialPort } = require('serialport');
const fs = require('fs');
const path = require('path');
const os = require('os');

const app = express();
app.use(express.json());

// ===== 설정 =====
const CONFIG_PATH = path.join(__dirname, 'config.json');
let config = {
    serial: { port: 'auto', baudRate: 115200 },
    server: { port: 3000 }
};

function loadConfig() {
    try {
        if (fs.existsSync(CONFIG_PATH)) {
            config = JSON.parse(fs.readFileSync(CONFIG_PATH, 'utf8'));
        }
    } catch (err) {}
}

function saveConfig() {
    try { fs.writeFileSync(CONFIG_PATH, JSON.stringify(config, null, 2)); } catch (err) {}
}

loadConfig();

const args = process.argv.slice(2);
for (let i = 0; i < args.length; i++) {
    if ((args[i] === '--port' || args[i] === '-p') && args[i + 1]) config.serial.port = args[i + 1];
    if ((args[i] === '--baud' || args[i] === '-b') && args[i + 1]) config.serial.baudRate = parseInt(args[i + 1]);
}

// ===== 상태 =====
let serialPort = null;
let status = {
    connected: false,
    port: '',
    baudRate: config.serial.baudRate,
    // 시스템 상태
    systemState: 0,      // 0:홈대기, 1:전진중, 2:도착-물건제거대기, 3:복귀중, 4:수동모드
    targetTable: 0,      // 목표 테이블 (0, 1, 2, 3)
    zoneCount: 0,        // 현재 구간 (zone_count)
    // 센서
    sen1: 0,
    sen2: 0,
    itemSensor: 0,       // 물건 감지 (0: 없음, 1: 있음)
    // 상태 텍스트
    stateText: '홈 대기',
    direction: 'STOP',
    // 로그
    lastMessage: '',
    eventLog: []
};

const STATE_NAMES = {
    0: '홈 대기',
    1: '전진 중',
    2: '도착 - 물건 제거 대기',
    3: '홈 복귀 중',
    4: '수동 모드'
};

const MAX_LOG = 30;

function addLog(msg) {
    const time = new Date().toLocaleTimeString();
    status.eventLog.unshift(`[${time}] ${msg}`);
    if (status.eventLog.length > MAX_LOG) status.eventLog.pop();
}

// ===== 시리얼 =====
async function listPorts() {
    try { return await SerialPort.list(); } catch (err) { return []; }
}

async function findPort() {
    const ports = await listPorts();
    if (ports.length === 0) return null;
    const target = ports.find(p => {
        const info = ((p.manufacturer || '') + (p.friendlyName || '')).toLowerCase();
        return ['serial', 'uart', 'ch340', 'cp210', 'ftdi', 'usb', 'st-link'].some(k => info.includes(k));
    });
    return target ? target.path : ports[0].path;
}

async function connect(portPath, baudRate) {
    if (serialPort) {
        try {
            if (serialPort.isOpen) {
                await new Promise(resolve => serialPort.close(() => resolve()));
                await new Promise(resolve => setTimeout(resolve, 500));
            }
        } catch (e) {}
        serialPort = null;
    }
    
    if (!portPath || portPath === 'auto') {
        portPath = await findPort();
        if (!portPath) { status.connected = false; return false; }
    }
    
    baudRate = baudRate || config.serial.baudRate;
    
    return new Promise(resolve => {
        try {
            serialPort = new SerialPort({ path: portPath, baudRate: baudRate, autoOpen: false });
            
            serialPort.on('error', err => { status.connected = false; });
            serialPort.on('close', () => { status.connected = false; });
            
            let buffer = '';
            serialPort.on('data', data => {
                buffer += data.toString();
                const lines = buffer.split(/\r?\n/);
                buffer = lines.pop();
                lines.forEach(line => {
                    if (line.trim()) {
                        console.log('📥', line);
                        parseSTM32(line);
                    }
                });
            });
            
            serialPort.open(err => {
                if (err) {
                    status.connected = false;
                    serialPort = null;
                    resolve(false);
                } else {
                    console.log(`✅ 연결: ${portPath} @ ${baudRate}`);
                    status.connected = true;
                    status.port = portPath;
                    status.baudRate = baudRate;
                    config.serial.port = portPath;
                    config.serial.baudRate = baudRate;
                    saveConfig();
                    resolve(true);
                }
            });
        } catch (err) {
            status.connected = false;
            resolve(false);
        }
    });
}

// STM32 응답 파싱
function parseSTM32(line) {
    status.lastMessage = line;
    
    // ST=1, TBL=2, ZONE=3, SEN=(1,0), LOAD
    const stMatch = line.match(/ST=(\d+)/);
    const tblMatch = line.match(/TBL=(\d+)/);
    const zoneMatch = line.match(/ZONE=(\d+)/);
    const senMatch = line.match(/SEN=\((\d+),(\d+)\)/);
    
    // LOAD/UNLOAD 텍스트 파싱
    const hasLoad = line.includes('LOAD') && !line.includes('UNLOAD');
    const hasUnload = line.includes('UNLOAD');
    
    if (stMatch) {
        const newState = parseInt(stMatch[1]);
        if (newState !== status.systemState) {
            status.systemState = newState;
            status.stateText = STATE_NAMES[newState] || '알 수 없음';
            addLog(`상태 변경: ${status.stateText}`);
        }
    }
    if (tblMatch) {
        const newTable = parseInt(tblMatch[1]);
        if (newTable !== status.targetTable) {
            status.targetTable = newTable;
            if (newTable > 0) addLog(`목표: 테이블 ${newTable}`);
        }
    }
    if (zoneMatch) {
        status.zoneCount = parseInt(zoneMatch[1]);
    }
    if (senMatch) {
        status.sen1 = parseInt(senMatch[1]);
        status.sen2 = parseInt(senMatch[2]);
    }
    
    // LOAD/UNLOAD 상태 업데이트
    if (hasLoad) {
        if (status.itemSensor !== 1) {
            status.itemSensor = 1;
            // ">>> LOAD" 메시지일 때만 로그
            if (line.includes('>>>')) {
                addLog('📦 물건 감지됨 (LOAD)');
            }
        }
    } else if (hasUnload) {
        if (status.itemSensor !== 0) {
            status.itemSensor = 0;
            if (line.includes('>>>')) {
                addLog('📭 물건 제거됨 (UNLOAD)');
            }
        }
    }
    
    // 이벤트 메시지 파싱
    if (line.includes('테이블') && line.includes('출발')) {
        const tableMatch = line.match(/테이블 (\d)/);
        if (tableMatch) addLog(`🚀 테이블 ${tableMatch[1]}로 출발!`);
    }
    if (line.includes('도착') && !line.includes('홈')) {
        const tableMatch = line.match(/테이블 (\d)/);
        if (tableMatch) {
            addLog(`🏁 테이블 ${tableMatch[1]} 도착!`);
        }
    }
    if (line.includes('홈 도착')) {
        addLog('🏠 홈 도착!');
    }
    if (line.includes('복귀 시작')) {
        addLog('🔙 홈으로 복귀 시작');
    }
    if (line.includes('비상 정지')) {
        addLog('🛑 비상 정지!');
    }
    
    // 구간 진입 로그
    const zoneEnterMatch = line.match(/(\d)구간 진입/);
    if (zoneEnterMatch) {
        addLog(`📍 ${zoneEnterMatch[1]}구간 진입`);
    }
    
    // 방향 상태
    if (status.systemState === 1) status.direction = 'FORWARD';
    else if (status.systemState === 3) status.direction = 'BACKWARD';
    else if (status.systemState === 4) status.direction = 'MANUAL';
    else status.direction = 'STOP';
}

async function disconnect() {
    if (serialPort) {
        try { if (serialPort.isOpen) await new Promise(r => serialPort.close(() => r())); } catch (e) {}
        serialPort = null;
        status.connected = false;
    }
}

function send(cmd) {
    if (!serialPort || !serialPort.isOpen) return false;
    serialPort.write(cmd);
    console.log('📤', cmd);
    return true;
}

// ===== HTML =====
const HTML = `
<!DOCTYPE html>
<html lang="ko">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>Serving Robot v1.3</title>
    <style>
        * { margin:0; padding:0; box-sizing:border-box; user-select:none; }
        :root { --bg:#0a0f1a; --card:#141d2b; --border:#2a3a50; --text:#e2e8f0; --dim:#64748b; --blue:#3b82f6; --green:#22c55e; --red:#ef4444; --orange:#f97316; --yellow:#eab308; --cyan:#06b6d4; --purple:#a855f7; }
        html,body { height:100%; overflow-x:hidden; }
        body { font-family:'Segoe UI',system-ui,sans-serif; background:var(--bg); color:var(--text); padding:10px; }
        
        .container { max-width:800px; margin:0 auto; display:flex; flex-direction:column; gap:10px; }
        
        /* 헤더 */
        .header { display:flex; justify-content:space-between; align-items:center; padding:10px 15px; background:var(--card); border-radius:12px; border:1px solid var(--border); }
        h1 { font-size:1.2rem; background:linear-gradient(135deg,#f59e0b,#ef4444); -webkit-background-clip:text; -webkit-text-fill-color:transparent; }
        .conn-badge { display:flex; align-items:center; gap:8px; }
        .header-btn { background:var(--border); border:none; color:var(--text); padding:6px 10px; border-radius:6px; cursor:pointer; }
        
        /* 상태 카드 */
        .status-card { background:var(--card); border-radius:12px; padding:15px; border:1px solid var(--border); }
        .status-header { display:flex; justify-content:space-between; align-items:center; margin-bottom:12px; }
        .status-title { font-size:0.8rem; color:var(--dim); }
        .status-main { text-align:center; padding:15px 0; }
        .state-text { font-size:1.5rem; font-weight:700; margin-bottom:8px; }
        .state-text.idle { color:var(--green); }
        .state-text.moving { color:var(--cyan); animation:pulse 1s infinite; }
        .state-text.waiting { color:var(--orange); }
        .state-text.returning { color:var(--purple); animation:pulse 1s infinite; }
        .state-text.manual { color:var(--yellow); }
        @keyframes pulse { 0%,100%{opacity:1} 50%{opacity:0.7} }
        
        .status-grid { display:grid; grid-template-columns:repeat(5,1fr); gap:10px; margin-top:15px; }
        .status-item { background:var(--bg); padding:10px; border-radius:8px; text-align:center; }
        .status-label { font-size:0.65rem; color:var(--dim); margin-bottom:4px; }
        .status-value { font-size:1rem; font-weight:600; }
        .status-value.active { color:var(--green); }
        .status-value.inactive { color:var(--dim); }
        
        /* 로봇 경로 시각화 */
        .path-panel { background:var(--card); border-radius:12px; padding:15px; border:1px solid var(--border); }
        .path-title { font-size:0.8rem; color:var(--dim); margin-bottom:10px; text-align:center; }
        #pathCanvas { width:100%; height:200px; display:block; }
        
        /* 테이블 선택 */
        .table-panel { background:var(--card); border-radius:12px; padding:15px; border:1px solid var(--border); }
        .panel-title { font-size:0.8rem; color:var(--dim); margin-bottom:10px; text-align:center; }
        .table-buttons { display:flex; justify-content:center; gap:12px; }
        .table-btn { width:80px; height:80px; border:none; border-radius:12px; font-size:1.5rem; cursor:pointer; display:flex; flex-direction:column; align-items:center; justify-content:center; background:var(--border); color:var(--text); transition:all 0.2s; }
        .table-btn:hover { transform:scale(1.05); }
        .table-btn.selected { background:var(--orange); color:white; box-shadow:0 0 20px rgba(249,115,22,0.4); }
        .table-btn.active { background:var(--green); animation:glow 1s infinite; }
        @keyframes glow { 0%,100%{box-shadow:0 0 10px rgba(34,197,94,0.5)} 50%{box-shadow:0 0 25px rgba(34,197,94,0.8)} }
        .table-btn span { font-size:0.7rem; margin-top:4px; }
        
        /* 물건 감지 */
        .item-panel { background:var(--card); border-radius:12px; padding:15px; border:1px solid var(--border); }
        .item-status { display:flex; align-items:center; justify-content:center; gap:20px; margin-bottom:15px; }
        .item-icon { font-size:3rem; }
        .item-text { font-size:1.2rem; font-weight:600; }
        .item-text.loaded { color:var(--green); }
        .item-text.empty { color:var(--dim); }
        .item-buttons { display:flex; justify-content:center; gap:10px; }
        .item-btn { padding:12px 25px; border:none; border-radius:8px; font-size:1rem; cursor:pointer; font-weight:600; }
        .item-btn.load { background:var(--green); color:white; }
        .item-btn.unload { background:var(--red); color:white; }
        .item-btn:disabled { opacity:0.5; cursor:not-allowed; }
        
        /* 컨트롤 */
        .control-panel { background:var(--card); border-radius:12px; padding:15px; border:1px solid var(--border); }
        .control-grid { display:grid; grid-template-columns:1fr 1fr; gap:15px; }
        .control-section { }
        .section-title { font-size:0.7rem; color:var(--dim); margin-bottom:8px; text-align:center; }
        
        .move-pad { display:flex; flex-direction:column; align-items:center; gap:4px; }
        .move-row { display:flex; gap:4px; }
        .move-btn { width:45px; height:45px; border:none; border-radius:8px; font-size:1rem; cursor:pointer; background:var(--blue); color:white; }
        .move-btn:active { transform:scale(0.95); }
        .move-btn.stop { background:var(--red); }
        
        .action-buttons { display:flex; flex-direction:column; gap:8px; }
        .action-btn { padding:12px; border:none; border-radius:8px; font-size:0.9rem; cursor:pointer; font-weight:500; }
        .action-btn.home { background:var(--purple); color:white; }
        .action-btn.emergency { background:var(--red); color:white; }
        
        /* 로그 */
        .log-panel { background:var(--card); border-radius:10px; padding:10px; border:1px solid var(--border); }
        .log-header { display:flex; justify-content:space-between; align-items:center; margin-bottom:6px; }
        .log-title { font-size:0.7rem; color:var(--dim); }
        .log-clear { background:var(--border); border:none; color:var(--dim); padding:2px 6px; border-radius:3px; cursor:pointer; font-size:0.6rem; }
        .log-content { font-family:monospace; font-size:0.65rem; max-height:120px; overflow-y:auto; }
        .log-line { padding:2px 0; border-bottom:1px solid var(--border); color:var(--cyan); }
        
        /* 모달 */
        .modal { display:none; position:fixed; top:0; left:0; width:100%; height:100%; background:rgba(0,0,0,0.8); justify-content:center; align-items:center; z-index:100; }
        .modal.show { display:flex; }
        .modal-content { background:var(--card); border-radius:16px; padding:20px; width:90%; max-width:350px; border:1px solid var(--border); }
        .modal-header { display:flex; justify-content:space-between; align-items:center; margin-bottom:15px; }
        .modal-title { font-size:1rem; font-weight:600; }
        .close-btn { background:none; border:none; color:var(--dim); font-size:1.3rem; cursor:pointer; }
        .form-group { margin-bottom:12px; }
        .form-label { display:block; color:var(--dim); font-size:0.75rem; margin-bottom:4px; }
        .form-select { width:100%; padding:8px; border-radius:6px; border:1px solid var(--border); background:var(--bg); color:var(--text); }
        .port-list { max-height:120px; overflow-y:auto; margin-bottom:8px; }
        .port-item { padding:8px 10px; background:var(--bg); border-radius:5px; margin-bottom:4px; cursor:pointer; border:1px solid var(--border); font-size:0.85rem; }
        .port-item:hover { border-color:var(--blue); }
        .port-item.selected { border-color:var(--green); background:rgba(34,197,94,0.1); }
        .btn-row { display:flex; gap:8px; margin-top:12px; }
        .btn-primary { flex:1; padding:10px; border:none; border-radius:8px; background:var(--blue); color:white; cursor:pointer; }
        .btn-danger { flex:1; padding:10px; border:none; border-radius:8px; background:var(--red); color:white; cursor:pointer; }
        .refresh-btn { background:var(--border); border:none; color:var(--text); padding:3px 6px; border-radius:4px; cursor:pointer; font-size:0.7rem; }
        
        /* RAW 데이터 */
        .raw-data { font-family:monospace; font-size:0.6rem; color:var(--dim); text-align:center; margin-top:5px; word-break:break-all; }
        
        @media (max-width:500px) {
            .status-grid { grid-template-columns:repeat(3,1fr); }
            .table-btn { width:70px; height:70px; font-size:1.3rem; }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🍽️ Serving Robot v1.3</h1>
            <div class="conn-badge">
                <span id="connPort" style="font-size:0.7rem;color:var(--dim);"></span>
                <button class="header-btn" onclick="openSettings()">⚙️</button>
                <span id="connStatus" style="font-size:1.3rem;">○</span>
            </div>
        </div>
        
        <div class="status-card">
            <div class="status-header">
                <span class="status-title">시스템 상태</span>
                <span class="status-title" id="directionBadge">STOP</span>
            </div>
            <div class="status-main">
                <div class="state-text" id="stateText">홈 대기</div>
                <div style="color:var(--dim);font-size:0.8rem;" id="stateDesc">물건을 올리고 테이블을 선택하세요</div>
            </div>
            <div class="status-grid">
                <div class="status-item">
                    <div class="status-label">목표 테이블</div>
                    <div class="status-value" id="targetTable">-</div>
                </div>
                <div class="status-item">
                    <div class="status-label">현재 구간</div>
                    <div class="status-value" id="zoneCount">0</div>
                </div>
                <div class="status-item">
                    <div class="status-label">물건</div>
                    <div class="status-value" id="itemStatus">-</div>
                </div>
                <div class="status-item">
                    <div class="status-label">센서 1</div>
                    <div class="status-value" id="sen1">0</div>
                </div>
                <div class="status-item">
                    <div class="status-label">센서 2</div>
                    <div class="status-value" id="sen2">0</div>
                </div>
            </div>
            <div class="raw-data" id="rawData">-</div>
        </div>
        
        <div class="path-panel">
            <div class="path-title">🛤️ 경로 시각화</div>
            <canvas id="pathCanvas"></canvas>
        </div>
        
        <div class="table-panel">
            <div class="panel-title">🎯 테이블 선택 (홈 대기 또는 수동 모드에서 가능)</div>
            <div class="table-buttons">
                <button class="table-btn" id="table1" onclick="sendCmd('1')">
                    🍽️
                    <span>테이블 1</span>
                </button>
                <button class="table-btn" id="table2" onclick="sendCmd('2')">
                    🍽️
                    <span>테이블 2</span>
                </button>
                <button class="table-btn" id="table3" onclick="sendCmd('3')">
                    🍽️
                    <span>테이블 3</span>
                </button>
            </div>
        </div>
        
        <div class="item-panel">
            <div class="panel-title">📦 물건 감지</div>
            <div class="item-status">
                <div class="item-icon" id="itemIcon">📭</div>
                <div class="item-text empty" id="itemText">UNLOAD</div>
            </div>
            <div class="item-buttons">
                <button class="item-btn load" id="btnLoad" onclick="sendCmd('L')">📦 LOAD (L)</button>
                <button class="item-btn unload" id="btnUnload" onclick="sendCmd('U')">📭 UNLOAD (U)</button>
            </div>
        </div>
        
        <div class="control-panel">
            <div class="control-grid">
                <div class="control-section">
                    <div class="section-title">수동 조작 (자동 모드 해제됨)</div>
                    <div class="move-pad">
                        <div class="move-row">
                            <button class="move-btn" onclick="sendCmd('q')">↖</button>
                            <button class="move-btn" onclick="sendCmd('W')">▲</button>
                            <button class="move-btn" onclick="sendCmd('e')">↗</button>
                        </div>
                        <div class="move-row">
                            <button class="move-btn" onclick="sendCmd('A')">◀</button>
                            <button class="move-btn stop" onclick="sendCmd('X')">■</button>
                            <button class="move-btn" onclick="sendCmd('D')">▶</button>
                        </div>
                        <div class="move-row">
                            <button class="move-btn" onclick="sendCmd('z')">↙</button>
                            <button class="move-btn" onclick="sendCmd('S')">▼</button>
                            <button class="move-btn" onclick="sendCmd('c')">↘</button>
                        </div>
                    </div>
                </div>
                <div class="control-section">
                    <div class="section-title">시스템 제어</div>
                    <div class="action-buttons">
                        <button class="action-btn home" onclick="sendCmd('0')">🏠 강제 홈 복귀 (0)</button>
                        <button class="action-btn emergency" onclick="sendCmd('X')">🛑 비상 정지 (X)</button>
                    </div>
                </div>
            </div>
        </div>
        
        <div class="log-panel">
            <div class="log-header">
                <span class="log-title">📋 이벤트 로그</span>
                <button class="log-clear" onclick="clearLog()">Clear</button>
            </div>
            <div class="log-content" id="logContent"></div>
        </div>
    </div>
    
    <div class="modal" id="settingsModal">
        <div class="modal-content">
            <div class="modal-header">
                <span class="modal-title">⚙️ 연결 설정</span>
                <button class="close-btn" onclick="closeSettings()">×</button>
            </div>
            <div class="form-group">
                <div style="display:flex;justify-content:space-between;margin-bottom:4px;">
                    <label class="form-label">시리얼 포트</label>
                    <button class="refresh-btn" onclick="refreshPorts()">🔄 새로고침</button>
                </div>
                <div class="port-list" id="portList"></div>
            </div>
            <div class="form-group">
                <label class="form-label">Baud Rate</label>
                <select class="form-select" id="baudRate">
                    <option value="9600">9600</option>
                    <option value="115200" selected>115200</option>
                </select>
            </div>
            <div class="btn-row">
                <button class="btn-primary" onclick="connectPort()">연결</button>
                <button class="btn-danger" onclick="disconnectPort()">해제</button>
            </div>
        </div>
    </div>

    <script>
        const canvas = document.getElementById('pathCanvas');
        const ctx = canvas.getContext('2d');
        let selectedPort = null;
        
        function resizeCanvas() {
            canvas.width = canvas.parentElement.clientWidth - 30;
            canvas.height = 200;
        }
        resizeCanvas();
        window.addEventListener('resize', resizeCanvas);
        
        // 경로 시각화
        function drawPath(data) {
            const W = canvas.width, H = canvas.height;
            ctx.fillStyle = '#0a0f1a';
            ctx.fillRect(0, 0, W, H);
            
            const padding = 40;
            const pathW = W - padding * 2;
            const pathY = H * 0.65;
            const tableY = H * 0.25;
            
            // 구간 위치 계산
            // zone: 0=홈, 1=(1,1), 2=(1,0)T1, 3=(0,1)T2, 4=(0,0)T3
            const zoneX = [
                padding + 20,                    // 홈 (0,0)
                padding + pathW * 0.25,          // 구간1 (1,1)
                padding + pathW * 0.45,          // 구간2 (1,0) - 테이블1
                padding + pathW * 0.65,          // 구간3 (0,1) - 테이블2
                padding + pathW * 0.85           // 구간4 (0,0) - 테이블3
            ];
            
            // 메인 경로 그리기
            ctx.strokeStyle = '#2a3a50';
            ctx.lineWidth = 12;
            ctx.lineCap = 'round';
            ctx.beginPath();
            ctx.moveTo(zoneX[0], pathY);
            ctx.lineTo(zoneX[4], pathY);
            ctx.stroke();
            
            // 테이블 분기선
            ctx.strokeStyle = '#1e3a5f';
            ctx.lineWidth = 4;
            ctx.setLineDash([5, 5]);
            [[2, 0], [3, 1], [4, 2]].forEach(([zone, table]) => {
                ctx.beginPath();
                ctx.moveTo(zoneX[zone], pathY);
                ctx.lineTo(zoneX[zone], tableY);
                ctx.stroke();
            });
            ctx.setLineDash([]);
            
            // 구간 센서 정보 (수정됨)
            const sensorInfo = ['(0,0)', '(1,1)', '(1,0)', '(0,1)', '(0,0)'];
            const zoneNames = ['홈', '구간1', 'T1', 'T2', 'T3'];
            
            // 구간 마커 그리기
            zoneX.forEach((x, i) => {
                // zone_count: 0=홈, 1=구간1, 2=T1, 3=T2, 4=T3
                const isCurrentZone = (data.zoneCount === i) || (data.zoneCount === 0 && i === 0);
                
                ctx.beginPath();
                ctx.arc(x, pathY, isCurrentZone ? 10 : 7, 0, Math.PI * 2);
                ctx.fillStyle = isCurrentZone ? '#22c55e' : '#3b5a80';
                ctx.fill();
                if (isCurrentZone) {
                    ctx.strokeStyle = '#fff';
                    ctx.lineWidth = 2;
                    ctx.stroke();
                }
                
                // 구간 라벨
                ctx.fillStyle = '#64748b';
                ctx.font = '10px sans-serif';
                ctx.textAlign = 'center';
                ctx.fillText(zoneNames[i], x, pathY + 25);
                ctx.fillText(sensorInfo[i], x, pathY + 38);
            });
            
            // 테이블 마커
            [0, 1, 2].forEach(i => {
                const x = zoneX[i + 2];
                const isTarget = data.targetTable === (i + 1);
                const isArrived = data.systemState === 2 && isTarget;
                
                ctx.beginPath();
                ctx.arc(x, tableY, 22, 0, Math.PI * 2);
                ctx.fillStyle = isArrived ? '#22c55e' : isTarget ? '#f59e0b' : '#1e293b';
                ctx.fill();
                ctx.strokeStyle = isArrived ? '#22c55e' : isTarget ? '#f59e0b' : '#3b5a80';
                ctx.lineWidth = 3;
                ctx.stroke();
                
                ctx.fillStyle = '#fff';
                ctx.font = 'bold 14px sans-serif';
                ctx.textAlign = 'center';
                ctx.fillText('T' + (i + 1), x, tableY + 5);
            });
            
            // 로봇 위치 계산
            let robotX = zoneX[0];
            let robotY = pathY;
            
            if (data.systemState === 0 || data.zoneCount === 0) {
                // 홈 대기
                robotX = zoneX[0];
                robotY = pathY;
            } else if (data.systemState === 2) {
                // 테이블에서 대기 중
                const tableIdx = data.targetTable + 1;  // zoneX index: T1=2, T2=3, T3=4
                robotX = zoneX[tableIdx];
                robotY = tableY + 35;
            } else {
                // 이동 중 - zoneCount 직접 사용 (0,1,2,3,4)
                const zoneIdx = Math.max(0, Math.min(4, data.zoneCount));
                robotX = zoneX[zoneIdx];
                robotY = pathY;
            }
            
            // 로봇 그리기
            ctx.beginPath();
            ctx.arc(robotX, robotY, 14, 0, Math.PI * 2);
            const robotColor = data.systemState === 1 ? '#3b82f6' : 
                              data.systemState === 3 ? '#a855f7' : 
                              data.systemState === 2 ? '#f59e0b' : 
                              data.systemState === 4 ? '#eab308' : '#22c55e';
            ctx.fillStyle = robotColor;
            ctx.fill();
            ctx.strokeStyle = '#fff';
            ctx.lineWidth = 3;
            ctx.stroke();
            
            // 로봇 방향/상태 표시
            ctx.fillStyle = '#fff';
            ctx.font = 'bold 12px sans-serif';
            ctx.textAlign = 'center';
            if (data.systemState === 1) ctx.fillText('→', robotX, robotY + 4);
            else if (data.systemState === 3) ctx.fillText('←', robotX, robotY + 4);
            else if (data.systemState === 2) ctx.fillText('⏸', robotX, robotY + 4);
            else if (data.systemState === 4) ctx.fillText('M', robotX, robotY + 4);
            else ctx.fillText('●', robotX, robotY + 4);
            
            // 물건 상태 표시
            if (data.itemSensor) {
                ctx.fillStyle = '#22c55e';
                ctx.font = '16px sans-serif';
                ctx.fillText('📦', robotX + 20, robotY - 10);
            }
        }
        
        function updateUI(data) {
            // 연결 상태
            const conn = document.getElementById('connStatus');
            conn.textContent = data.connected ? '●' : '○';
            conn.style.color = data.connected ? '#22c55e' : '#ef4444';
            document.getElementById('connPort').textContent = data.connected ? data.port : '';
            
            // 상태 텍스트
            const stateEl = document.getElementById('stateText');
            stateEl.textContent = data.stateText;
            stateEl.className = 'state-text ' + ['idle', 'moving', 'waiting', 'returning', 'manual'][data.systemState];
            
            // 방향 배지
            const dirBadge = document.getElementById('directionBadge');
            dirBadge.textContent = data.direction;
            dirBadge.style.color = data.direction === 'FORWARD' ? '#3b82f6' : 
                                   data.direction === 'BACKWARD' ? '#a855f7' : 
                                   data.direction === 'MANUAL' ? '#eab308' : '#64748b';
            
            // 상태 설명
            const descEl = document.getElementById('stateDesc');
            const descs = [
                data.targetTable ? '테이블 ' + data.targetTable + ' 선택됨 - 물건을 올려주세요' : '물건을 올리고 테이블을 선택하세요',
                '테이블 ' + data.targetTable + '로 이동 중... (구간 ' + data.zoneCount + ')',
                '물건을 내려주세요',
                '홈으로 복귀 중... (구간 ' + data.zoneCount + ')',
                '수동 조작 모드 - 1/2/3으로 자동모드 전환'
            ];
            descEl.textContent = descs[data.systemState];
            
            // 상태 값들
            document.getElementById('targetTable').textContent = data.targetTable || '-';
            document.getElementById('zoneCount').textContent = data.zoneCount || '홈';
            document.getElementById('sen1').textContent = data.sen1;
            document.getElementById('sen1').className = 'status-value ' + (data.sen1 ? 'active' : 'inactive');
            document.getElementById('sen2').textContent = data.sen2;
            document.getElementById('sen2').className = 'status-value ' + (data.sen2 ? 'active' : 'inactive');
            
            // 물건 상태
            const itemStatusEl = document.getElementById('itemStatus');
            itemStatusEl.textContent = data.itemSensor ? 'LOAD' : 'UNLOAD';
            itemStatusEl.className = 'status-value ' + (data.itemSensor ? 'active' : 'inactive');
            
            // RAW 데이터
            document.getElementById('rawData').textContent = data.lastMessage || '-';
            
            // 테이블 버튼 (홈 대기 또는 수동모드에서 활성화)
            const canSelect = data.systemState === 0 || data.systemState === 4;
            [1, 2, 3].forEach(i => {
                const btn = document.getElementById('table' + i);
                btn.className = 'table-btn';
                btn.disabled = !canSelect;
                btn.style.opacity = canSelect ? '1' : '0.5';
                if (data.targetTable === i) {
                    btn.classList.add(data.systemState === 2 ? 'active' : 'selected');
                }
            });
            
            // 물건 감지
            const itemIcon = document.getElementById('itemIcon');
            const itemText = document.getElementById('itemText');
            if (data.itemSensor) {
                itemIcon.textContent = '📦';
                itemText.textContent = 'LOAD';
                itemText.className = 'item-text loaded';
            } else {
                itemIcon.textContent = '📭';
                itemText.textContent = 'UNLOAD';
                itemText.className = 'item-text empty';
            }
            
            // 로그
            const logEl = document.getElementById('logContent');
            if (data.eventLog && data.eventLog.length > 0) {
                logEl.innerHTML = data.eventLog.map(l => '<div class="log-line">' + l + '</div>').join('');
            }
            
            // 경로 그리기
            drawPath(data);
        }
        
        function sendCmd(cmd) {
            fetch('/api/cmd?c=' + cmd).then(r => r.json()).then(updateUI);
        }
        
        function clearLog() {
            fetch('/api/log/clear', { method: 'POST' });
        }
        
        // 키보드
        document.addEventListener('keydown', e => {
            if (e.target.tagName === 'INPUT' || e.target.tagName === 'SELECT') return;
            
            const keyMap = {
                'w':'W', 'W':'W', 'ArrowUp':'W',
                's':'S', 'S':'S', 'ArrowDown':'S',
                'a':'A', 'A':'A', 'ArrowLeft':'A',
                'd':'D', 'D':'D', 'ArrowRight':'D',
                'q':'q', 'Q':'q', 'e':'e', 'E':'e',
                'z':'z', 'Z':'z', 'c':'c', 'C':'c',
                '1':'1', '2':'2', '3':'3', '0':'0',
                'x':'X', 'X':'X', ' ':'X',
                'l':'L', 'L':'L', 'u':'U', 'U':'U'
            };
            if (keyMap[e.key]) {
                e.preventDefault();
                sendCmd(keyMap[e.key]);
            }
        });
        
        // 설정
        function openSettings() { document.getElementById('settingsModal').classList.add('show'); refreshPorts(); }
        function closeSettings() { document.getElementById('settingsModal').classList.remove('show'); }
        function refreshPorts() {
            fetch('/api/ports').then(r => r.json()).then(ports => {
                document.getElementById('portList').innerHTML = ports.length ?
                    ports.map(p => '<div class="port-item" onclick="selPort(this,\\'' + p.path + '\\')"><b>' + p.path + '</b><br><span style="font-size:0.7rem;color:#64748b;">' + (p.manufacturer || p.friendlyName || '') + '</span></div>').join('') :
                    '<div style="color:#64748b;text-align:center;padding:12px;">포트가 없습니다</div>';
            });
        }
        function selPort(el, port) {
            document.querySelectorAll('.port-item').forEach(i => i.classList.remove('selected'));
            el.classList.add('selected');
            selectedPort = port;
        }
        function connectPort() {
            if (!selectedPort) return alert('포트를 선택하세요');
            fetch('/api/connect', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ port: selectedPort, baudRate: parseInt(document.getElementById('baudRate').value) })
            }).then(r => r.json()).then(d => { if (d.success) closeSettings(); });
        }
        function disconnectPort() { fetch('/api/disconnect', { method: 'POST' }); }
        document.getElementById('settingsModal').addEventListener('click', e => { if (e.target.id === 'settingsModal') closeSettings(); });
        
        // 폴링
        setInterval(() => fetch('/api/status').then(r => r.json()).then(updateUI).catch(() => {}), 150);
        
        // 초기화
        drawPath({ systemState: 0, targetTable: 0, zoneCount: 0, sen1: 0, sen2: 0, itemSensor: 0 });
    </script>
</body>
</html>
`;

// ===== 라우트 =====
app.get('/', (req, res) => res.send(HTML));
app.get('/api/cmd', (req, res) => { if (req.query.c) send(req.query.c); res.json(status); });
app.get('/api/status', (req, res) => res.json(status));
app.get('/api/ports', async (req, res) => res.json(await listPorts()));
app.post('/api/connect', async (req, res) => {
    const ok = await connect(req.body.port, req.body.baudRate);
    res.json({ success: ok });
});
app.post('/api/disconnect', async (req, res) => { await disconnect(); res.json({ success: true }); });
app.post('/api/log/clear', (req, res) => { status.eventLog = []; res.json({ success: true }); });

// ===== 시작 =====
async function start() {
    console.log('='.repeat(50));
    console.log('   🍽️  STM32 Serving Robot Controller v1.3');
    console.log('='.repeat(50));
    
    await connect(config.serial.port, config.serial.baudRate);
    
    app.listen(config.server.port, '0.0.0.0', () => {
        const nets = os.networkInterfaces();
        let ip = 'localhost';
        for (const n of Object.keys(nets)) {
            for (const net of nets[n]) {
                if (net.family === 'IPv4' && !net.internal) { ip = net.address; break; }
            }
        }
        console.log('🌐 http://localhost:' + config.server.port);
        console.log('📱 http://' + ip + ':' + config.server.port);
        console.log('='.repeat(50));
    });
}

start();
