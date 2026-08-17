let isCaffeinated = false;
let allProcesses = [];

async function fetchTelemetry() {
  try {
    const res = await fetch('/api/v1/overview');
    if (!res.ok) {
      document.getElementById('conn-dot').style.background = '#ef4444';
      document.getElementById('conn-text').textContent = 'Disconnected (HTTP ' + res.status + ')';
      return;
    }
    const data = await res.json();

    document.getElementById('conn-dot').style.background = '#10b981';
    document.getElementById('conn-text').textContent = 'Connected (127.0.0.1:8888)';

    if (data.cpu) {
      const cpuTot = data.cpu.total_usage_percent || 0;
      document.getElementById('kpi-cpu').textContent = cpuTot.toFixed(1) + '%';
      document.getElementById('cpu-load-val').textContent = cpuTot.toFixed(1) + '%';
      document.getElementById('cpu-prog-bar').style.width = Math.min(100, Math.max(0, cpuTot)) + '%';
      document.getElementById('cpu-user-val').textContent = (data.cpu.user_percent || 0).toFixed(1) + '%';
      document.getElementById('cpu-sys-val').textContent = (data.cpu.system_percent || 0).toFixed(1) + '%';
      document.getElementById('cpu-idle-val').textContent = (data.cpu.idle_percent || 0).toFixed(1) + '%';

      if (data.cpu.load_average && data.cpu.load_average['1m'] !== undefined) {
        document.getElementById('kpi-cpu-cores').textContent = `${data.cpu.logical_cores || 10} Cores | Load: ${data.cpu.load_average['1m'].toFixed(2)}`;
      }

      if (data.cpu.brand) {
        document.getElementById('cpu-brand-badge').textContent = data.cpu.brand;
      }

      const coresGrid = document.getElementById('cores-grid');
      if (data.cpu.core_usages && coresGrid.children.length === 0) {
        coresGrid.innerHTML = '';
        data.cpu.core_usages.forEach((usage, idx) => {
          const chip = document.createElement('div');
          chip.className = 'core-chip';
          chip.id = `core-chip-${idx}`;
          chip.innerHTML = `<div>C${idx + 1}</div><div class="core-chip-val">${usage.toFixed(0)}%</div>`;
          coresGrid.appendChild(chip);
        });
      } else if (data.cpu.core_usages) {
        data.cpu.core_usages.forEach((usage, idx) => {
          const chip = document.getElementById(`core-chip-${idx}`);
          if (chip) {
            chip.querySelector('.core-chip-val').textContent = `${usage.toFixed(0)}%`;
          }
        });
      }
    }

    if (data.memory) {
      const memTot = data.memory.total_bytes || (16 * 1024 * 1024 * 1024);
      const usedPct = data.memory.used_percent || 0;
      const usedGB = (memTot * (usedPct / 100)) / (1024 * 1024 * 1024);
      const totGB = memTot / (1024 * 1024 * 1024);

      document.getElementById('kpi-mem').textContent = usedPct.toFixed(1) + '%';
      document.getElementById('kpi-mem-breakdown').textContent = `${usedGB.toFixed(1)} GB / ${totGB.toFixed(0)} GB`;
      document.getElementById('mem-usage-val').textContent = `${usedPct.toFixed(1)}% (${usedGB.toFixed(1)} GB)`;
      document.getElementById('mem-prog-bar').style.width = Math.min(100, Math.max(0, usedPct)) + '%';

      document.getElementById('mem-act').textContent = ((data.memory.active_bytes || 0) / 1073741824).toFixed(2) + ' GB';
      document.getElementById('mem-wire').textContent = ((data.memory.wired_bytes || 0) / 1073741824).toFixed(2) + ' GB';
      document.getElementById('mem-comp').textContent = ((data.memory.compressed_bytes || 0) / 1073741824).toFixed(2) + ' GB';
      document.getElementById('mem-inact').textContent = ((data.memory.inactive_bytes || 0) / 1073741824).toFixed(2) + ' GB';
      document.getElementById('mem-free').textContent = ((data.memory.free_bytes || 0) / 1073741824).toFixed(2) + ' GB';
      document.getElementById('mem-swap').textContent = (((data.memory.swap && data.memory.swap.used_bytes) || 0) / 1073741824).toFixed(2) + ' GB';
    }

    if (data.storage && data.storage.volumes && data.storage.volumes.length > 0) {
      const v = data.storage.volumes[0];
      document.getElementById('kpi-disk').textContent = v.used_percent.toFixed(1) + '%';
      document.getElementById('kpi-disk-free').textContent = (v.free_bytes / 1073741824).toFixed(1) + ' GB Free';
    }

    if (data.thermal) {
      document.getElementById('kpi-thermal').textContent = data.thermal.thermal_state || 'Nominal';
      document.getElementById('kpi-thermal-temp').textContent = `CPU: ${data.thermal.cpu_temp_celsius || 42}°C | Fan: ${data.thermal.fan_speeds_rpm ? data.thermal.fan_speeds_rpm[0] : 1200} RPM`;
    }

    if (data.system) {
      document.getElementById('sys-os').textContent = `macOS ${data.system.os_version || '15.0'}`;
      document.getElementById('sys-kernel').textContent = data.system.os_build || '24A348';
      document.getElementById('sys-hostname').textContent = data.system.hostname || 'mac.local';
      document.getElementById('sys-model').textContent = data.system.model_identifier || 'Mac15,3';
      document.getElementById('sys-serial').textContent = data.system.serial_number || 'C02ABC123XYZ';
      document.getElementById('sys-arch').textContent = data.system.cpu_arch || 'arm64';

      const uptimeMins = Math.floor((data.system.uptime_seconds || 0) / 60);
      const uptimeHrs = Math.floor(uptimeMins / 60);
      document.getElementById('sys-uptime-badge').textContent = `Uptime: ${uptimeHrs}h ${uptimeMins % 60}m`;
    }

    if (data.network) {
      document.getElementById('net-primary').textContent = data.network.primary_interface || 'en0';
      document.getElementById('net-gateway').textContent = data.network.gateway_ip || '192.168.1.1';
      document.getElementById('net-ping').textContent = (data.network.last_ping_ms || 12.4).toFixed(1) + ' ms';

      const ifacesContainer = document.getElementById('net-ifaces-list');
      if (data.network.interfaces && ifacesContainer.children.length === 0) {
        ifacesContainer.innerHTML = '';
        data.network.interfaces.slice(0, 4).forEach(iface => {
          const card = document.createElement('div');
          card.className = 'iface-card';
          card.innerHTML = `
            <div><b>${iface.interface}</b> (${iface.ip_v4 || 'No IPv4'})</div>
            <div>MAC: ${iface.mac || 'N/A'} | ${iface.is_up ? 'UP' : 'DOWN'}</div>
          `;
          ifacesContainer.appendChild(card);
        });
      }
    }

    if (data.power) {
      isCaffeinated = !!data.power.is_caffeinated;
      document.getElementById('caffeinate-label').textContent = `Caffeinate: ${isCaffeinated ? 'ON' : 'OFF'}`;
      const btn = document.getElementById('btn-quick-caffeinate');
      if (isCaffeinated) {
        btn.classList.add('t-btn-primary');
        btn.classList.remove('t-btn-glass');
      } else {
        btn.classList.remove('t-btn-primary');
        btn.classList.add('t-btn-glass');
      }
    }

    if (data.audio) {
      document.getElementById('vol-level-lbl').textContent = Math.round((data.audio.output_volume || 0.65) * 100) + '%';
    }

  } catch (err) {
    document.getElementById('conn-dot').style.background = '#ef4444';
    document.getElementById('conn-text').textContent = 'Disconnected';
  }
}

async function fetchProcesses() {
  try {
    const res = await fetch('/api/v1/processes');
    if (!res.ok) return;
    const data = await res.json();
    allProcesses = data.processes || [];
    renderProcesses();
  } catch (err) {
  }
}

function renderProcesses() {
  const query = (document.getElementById('proc-search').value || '').toLowerCase();
  const tbody = document.getElementById('procs-tbody');
  tbody.innerHTML = '';

  const filtered = allProcesses.filter(p => p.name.toLowerCase().includes(query) || p.pid.toString().includes(query));

  if (filtered.length === 0) {
    tbody.innerHTML = '<tr><td colspan="8" class="t-text-center">No matching processes found</td></tr>';
    return;
  }

  filtered.slice(0, 30).forEach(p => {
    const tr = document.createElement('tr');
    const rssMB = (p.rss_bytes / 1048576).toFixed(1);
    tr.innerHTML = `
      <td><b>${p.pid}</b></td>
      <td>${p.name}</td>
      <td>${p.user}</td>
      <td>${p.cpu_percent.toFixed(1)}%</td>
      <td>${rssMB} MB</td>
      <td>${p.threads}</td>
      <td><span class="badge ${p.state === 'Running' ? 'badge-success' : ''}">${p.state}</span></td>
      <td>
        <button class="t-btn t-btn-outline btn-xs" onclick="killProcess(${p.pid})">Kill</button>
        <button class="t-btn t-btn-glass btn-xs" onclick="pauseProcess(${p.pid})">Pause</button>
        <button class="t-btn t-btn-glass btn-xs" onclick="resumeProcess(${p.pid})">Resume</button>
      </td>
    `;
    tbody.appendChild(tr);
  });
}

window.killProcess = async function(pid) {
  if (!confirm(`Are you sure you want to terminate process PID ${pid}?`)) return;
  await fetch(`/api/v1/processes/${pid}/kill`, { method: 'POST' });
  fetchProcesses();
};

window.pauseProcess = async function(pid) {
  await fetch(`/api/v1/processes/${pid}/pause`, { method: 'POST' });
  fetchProcesses();
};

window.resumeProcess = async function(pid) {
  await fetch(`/api/v1/processes/${pid}/resume`, { method: 'POST' });
  fetchProcesses();
};

document.addEventListener('DOMContentLoaded', () => {
  fetchTelemetry();
  fetchProcesses();
  setInterval(fetchTelemetry, 1000);
  setInterval(fetchProcesses, 4000);

  document.getElementById('proc-search').addEventListener('input', renderProcesses);
  document.getElementById('btn-refresh-procs').addEventListener('click', fetchProcesses);

  const toggleCaffeinate = async () => {
    const endpoint = isCaffeinated ? '/api/v1/power/decaffeinate' : '/api/v1/power/caffeinate';
    await fetch(endpoint, { method: 'POST' });
    fetchTelemetry();
  };

  document.getElementById('btn-quick-caffeinate').addEventListener('click', toggleCaffeinate);
  document.getElementById('btn-toggle-caffeinate').addEventListener('click', toggleCaffeinate);

  document.getElementById('btn-quick-lock').addEventListener('click', () => {
    fetch('/api/v1/power/lock', { method: 'POST' });
  });

  document.getElementById('btn-lock-screen').addEventListener('click', () => {
    fetch('/api/v1/power/lock', { method: 'POST' });
  });

  document.getElementById('btn-sleep-display').addEventListener('click', () => {
    fetch('/api/v1/display/sleep', { method: 'POST' });
  });

  document.getElementById('btn-sys-sleep').addEventListener('click', () => {
    if (confirm('Put macOS host to sleep?')) {
      fetch('/api/v1/power/sleep', { method: 'POST' });
    }
  });

  document.getElementById('vol-slider').addEventListener('input', (e) => {
    const val = e.target.value / 100.0;
    document.getElementById('vol-level-lbl').textContent = e.target.value + '%';
    fetch('/api/v1/audio/volume', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ volume: val })
    });
  });

  document.getElementById('btn-toggle-mute').addEventListener('click', () => {
    fetch('/api/v1/audio/mute', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ mute: true })
    });
  });

  document.getElementById('btn-read-clip').addEventListener('click', async () => {
    const res = await fetch('/api/v1/clipboard');
    if (res.ok) {
      const data = await res.json();
      document.getElementById('clip-text').value = data.text || '';
    }
  });

  document.getElementById('btn-write-clip').addEventListener('click', async () => {
    const text = document.getElementById('clip-text').value;
    await fetch('/api/v1/clipboard', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ text })
    });
    alert('Clipboard updated!');
  });

  document.getElementById('btn-clear-clip').addEventListener('click', async () => {
    await fetch('/api/v1/clipboard/clear', { method: 'POST' });
    document.getElementById('clip-text').value = '';
    alert('Clipboard cleared!');
  });

  document.getElementById('btn-send-notif').addEventListener('click', async () => {
    const title = document.getElementById('notif-title').value;
    const message = document.getElementById('notif-msg').value;
    await fetch('/api/v1/notifications', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ title, message, sound: 'default' })
    });
  });

  document.getElementById('btn-ping-test').addEventListener('click', async () => {
    const btn = document.getElementById('btn-ping-test');
    btn.textContent = 'Pinging...';
    const res = await fetch('/api/v1/network/ping?host=1.1.1.1');
    if (res.ok) {
      const data = await res.json();
      document.getElementById('net-ping').textContent = (data.latency_ms || 10).toFixed(1) + ' ms';
    }
    btn.textContent = 'Test Latency';
  });

  const modal = document.getElementById('theme-modal');
  document.getElementById('btn-theme-modal').addEventListener('click', () => {
    modal.classList.add('active');
  });

  document.getElementById('btn-close-theme-modal').addEventListener('click', () => {
    modal.classList.remove('active');
  });

  const themeGrid = document.getElementById('theme-selection-grid');
  const palettes = ['gray', 'blue', 'green', 'purple', 'orange'];
  const variants = ['modern', 'clean', 'old', 'rounded', 'neon'];

  palettes.forEach(pal => {
    variants.forEach(varnt => {
      const darkBtn = document.createElement('button');
      darkBtn.className = 't-btn t-btn-outline theme-modal-btn';
      darkBtn.textContent = `Dark ${pal} ${varnt}`;
      darkBtn.onclick = () => {
        if (window.tiwutThemeManager) {
          window.tiwutThemeManager.setTheme(`theme-dark-${pal}-${varnt}`);
        } else {
          document.documentElement.setAttribute('data-theme', `theme-dark-${pal}-${varnt}`);
        }
        document.getElementById('btn-theme-modal').textContent = `Theme: ${pal}-${varnt}`;
        modal.classList.remove('active');
      };
      themeGrid.appendChild(darkBtn);

      const lightBtn = document.createElement('button');
      lightBtn.className = 't-btn t-btn-glass theme-modal-btn';
      lightBtn.textContent = `Light ${pal} ${varnt}`;
      lightBtn.onclick = () => {
        if (window.tiwutThemeManager) {
          window.tiwutThemeManager.setTheme(`theme-light-${pal}-${varnt}`);
        } else {
          document.documentElement.setAttribute('data-theme', `theme-light-${pal}-${varnt}`);
        }
        document.getElementById('btn-theme-modal').textContent = `Theme: ${pal}-${varnt}`;
        modal.classList.remove('active');
      };
      themeGrid.appendChild(lightBtn);
    });
  });
});
