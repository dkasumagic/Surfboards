let generatingInterval = null;

function startGenerating(btn) {
  if (!btn) return;
  const label = btn.querySelector("#applyBtnLabel") || btn;
  let step = 0;
  btn.disabled = true;
  btn.classList.add("opacity-60","cursor-not-allowed");
  const base = "Generating";
  label.textContent = base + ".";
  generatingInterval = setInterval(() => {
    step = (step + 1) % 3;
    label.textContent = base + ".".repeat(step + 1);
  }, 450);
}

function stopGenerating(btn) {
  if (!btn) return;
  const label = btn.querySelector("#applyBtnLabel") || btn;
  if (generatingInterval) {
    clearInterval(generatingInterval);
    generatingInterval = null;
  }
  label.textContent = "Apply Matrices";
  btn.disabled = false;
  btn.classList.remove("opacity-60","cursor-not-allowed");
}

function showExclamationMark() {
  const exclamationMark = document.getElementById('exclamationMark');
  if (exclamationMark) {
    exclamationMark.classList.remove('hidden');
  }
}

function hideExclamationMark() {
  const exclamationMark = document.getElementById('exclamationMark');
  if (exclamationMark) {
    exclamationMark.classList.add('hidden');
  }
}

async function applyMatrices() {
  const applyBtn = document.getElementById("applyMatricesBtn");
  startGenerating(applyBtn);

  const ids = ["matrixA_rows", "matrixA_cols", "matrixB_rows", "matrixB_cols"];
  const values = ids.map(getInputValue);
  ids.forEach((id, i) => markValidity(id, values[i] !== null));

  if (values.some(v => v === null)) {
    stopGenerating(applyBtn);
    alert("Enter positive integer values for all matrix dimensions.");
    return;
  }

  const [r1, c1, r2, c2] = values;

  const q1 = document.getElementById("q_integer").value;
  const q2 = document.getElementById("q_fractional").value;
  const signed = document.getElementById("q_signed").checked ? 1 : 0;
  const useSVD = document.getElementById("use_svd").checked ? 1 : 0;

  try {
    if (typeof webui !== "undefined" && typeof webui.call === "function") {
      const result = await webui.call("inputMatrix", r1, c1, r2, c2, q1, q2, signed, useSVD);
      console.log("result:", result);

      // Show exclamation mark when matrices are successfully applied
      if (result === "ok") {
        showExclamationMark();
      }
    } else {
      console.warn("webui.call is not available.");
      // For testing without backend, show exclamation mark
      showExclamationMark();
    }
  } catch (err) {
    console.error("inputMatrix call failed", err);
  } finally {
    stopGenerating(applyBtn);
  }
}

function updateQFormatDisplay() {
  const qInt = document.getElementById("q_integer").value || "8";
  const qFrac = document.getElementById("q_fractional").value || "8";
  const formatText = document.getElementById("q-format-text");
  if (formatText) {
    formatText.textContent = `Fixed-point Q${qInt}.${qFrac}`;
  }
}

let currentZoom = 1;
let isFullscreen = false;

function initZoomControls() {
  const zoomIn = document.getElementById('zoomIn');
  const zoomOut = document.getElementById('zoomOut');
  const zoomReset = document.getElementById('zoomReset');
  const zoomLevel = document.getElementById('zoomLevel');
  const downloadSvg = document.getElementById('downloadSvg');
  const fullscreenBtn = document.getElementById('fullscreenBtn');
  const svgContainer = document.getElementById('svgContainer');

  if (!zoomIn || !zoomOut || !zoomReset || !zoomLevel || !svgContainer) return;

  const updateZoom = () => {
    const svgContent = svgContainer.querySelector('.svg-content');
    if (svgContent) {
      svgContent.style.transform = `scale(${currentZoom})`;
      svgContent.style.transformOrigin = 'center center';
    }
    zoomLevel.textContent = `${Math.round(currentZoom * 100)}%`;

    // Disable buttons at limits
    zoomOut.disabled = currentZoom <= 0.25;
    zoomIn.disabled = currentZoom >= 3;

    if (zoomOut.disabled) {
      zoomOut.classList.add('opacity-50', 'cursor-not-allowed');
    } else {
      zoomOut.classList.remove('opacity-50', 'cursor-not-allowed');
    }

    if (zoomIn.disabled) {
      zoomIn.classList.add('opacity-50', 'cursor-not-allowed');
    } else {
      zoomIn.classList.remove('opacity-50', 'cursor-not-allowed');
    }
  };

  // Download SVG functionality
  if (downloadSvg) {
    downloadSvg.addEventListener('click', () => {
      const svg = svgContainer.querySelector('svg');
      if (svg) {
        const svgData = new XMLSerializer().serializeToString(svg);
        const svgBlob = new Blob([svgData], { type: 'image/svg+xml;charset=utf-8' });
        const svgUrl = URL.createObjectURL(svgBlob);

        const downloadLink = document.createElement('a');
        downloadLink.href = svgUrl;
        downloadLink.download = 'circuit_diagram.svg';
        document.body.appendChild(downloadLink);
        downloadLink.click();
        document.body.removeChild(downloadLink);
        URL.revokeObjectURL(svgUrl);
      }
    });
  }

  // Fullscreen functionality
  if (fullscreenBtn) {
    fullscreenBtn.addEventListener('click', () => {
      const svgViewport = svgContainer; // Target the SVG container directly
      if (!isFullscreen) {
        if (svgViewport.requestFullscreen) {
          svgViewport.requestFullscreen();
        } else if (svgViewport.webkitRequestFullscreen) {
          svgViewport.webkitRequestFullscreen();
        } else if (svgViewport.msRequestFullscreen) {
          svgViewport.msRequestFullscreen();
        }
      } else {
        if (document.exitFullscreen) {
          document.exitFullscreen();
        } else if (document.webkitExitFullscreen) {
          document.webkitExitFullscreen();
        } else if (document.msExitFullscreen) {
          document.msExitFullscreen();
        }
      }
    });

    // Listen for fullscreen changes
    document.addEventListener('fullscreenchange', () => {
      isFullscreen = !!document.fullscreenElement;
      updateFullscreenIcon();
      updateFullscreenStyles();
    });
    document.addEventListener('webkitfullscreenchange', () => {
      isFullscreen = !!document.webkitFullscreenElement;
      updateFullscreenIcon();
      updateFullscreenStyles();
    });
    document.addEventListener('msfullscreenchange', () => {
      isFullscreen = !!document.msFullscreenElement;
      updateFullscreenIcon();
      updateFullscreenStyles();
    });

    const updateFullscreenStyles = () => {
      if (isFullscreen) {
        svgContainer.classList.add('fullscreen-svg');
      } else {
        svgContainer.classList.remove('fullscreen-svg');
      }
    };

    const updateFullscreenIcon = () => {
      const icon = fullscreenBtn.querySelector('svg');
      if (isFullscreen) {
        icon.innerHTML = `<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M9 9l6 6m0-6l-6 6M21 3l-6 6m0 0V4m0 5h5M3 21l6-6m0 0v5m0-5H4"/>`;
        fullscreenBtn.title = 'Exit Fullscreen';
      } else {
        icon.innerHTML = `<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M4 8V4m0 0h4M4 4l5 5m11-1V4m0 0h-4m4 0l-5 5M4 16v4m0 0h4m-4 0l5-5m11 5l-5-5m5 5v-4m0 4h-4"/>`;
        fullscreenBtn.title = 'Toggle Fullscreen';
      }
    };
  }

  zoomIn.addEventListener('click', () => {
    if (currentZoom < 3) {
      currentZoom = Math.min(3, currentZoom + 0.25);
      updateZoom();
    }
  });

  zoomOut.addEventListener('click', () => {
    if (currentZoom > 0.25) {
      currentZoom = Math.max(0.25, currentZoom - 0.25);
      updateZoom();
    }
  });

  zoomReset.addEventListener('click', () => {
    currentZoom = 1;
    updateZoom();
  });

  // Initial update
  updateZoom();
}

async function loadCircuitDiagram() {
  const svgContainer = document.getElementById('svgContainer');
  if (!svgContainer) return;

  // Show loading state
  svgContainer.innerHTML = `
    <div class="text-center">
      <div class="animate-spin w-8 h-8 border-2 border-neutral-400 border-t-transparent rounded-full mx-auto mb-4"></div>
      <span class="theme-text-muted">Loading circuit diagram...</span>
    </div>
  `;

  try {
    // Fetch the SVG file from build/schematic.svg
    const response = await fetch('public/schematic.svg');

    if (response.ok) {
      const svgContent = await response.text();
      // Display the SVG content with proper layout
      svgContainer.innerHTML = `
        <div class="w-full h-full overflow-auto">
          <div class="svg-content min-w-fit min-h-fit p-8 m-4 inline-block">
            ${svgContent}
          </div>
        </div>
      `;

      // Style the SVG for theme compatibility and sizing
      const svg = svgContainer.querySelector('svg');
      if (svg) {
        // Remove any hardcoded colors and sizes
        svg.style.maxWidth = 'none';
        svg.style.maxHeight = 'none';
        svg.style.width = 'auto';
        svg.style.height = 'auto';
        svg.style.display = 'block';

        // Apply theme colors to SVG elements
        svg.classList.add('theme-svg');

        // Override black colors in SVG
        const svgElements = svg.querySelectorAll('*');
        svgElements.forEach(el => {
          // Change black strokes to theme color
          if (el.getAttribute('stroke') === '#000000' || el.getAttribute('stroke') === 'black') {
            el.setAttribute('stroke', 'currentColor');
            el.classList.add('theme-text-primary');
          }
          // Change black fills to theme color
          if (el.getAttribute('fill') === '#000000' || el.getAttribute('fill') === 'black') {
            el.setAttribute('fill', 'currentColor');
            el.classList.add('theme-text-primary');
          }
          // Handle text elements
          if (el.tagName === 'text') {
            el.classList.add('theme-text-primary');
            el.style.fill = 'currentColor';
          }
        });
      }

      // Initialize zoom controls after SVG loads
      currentZoom = 1; // Reset zoom
      initZoomControls();
    } else {
      throw new Error(`SVG file not found (${response.status})`);
    }
  } catch (error) {
    console.error('Failed to load SVG:', error);
    // Show error state
    svgContainer.innerHTML = `
      <div class="text-center">
        <svg class="w-16 h-16 mx-auto theme-text-muted mb-4" fill="none" stroke="currentColor" viewBox="0 0 24 24">
          <path stroke-linecap="round" stroke-linejoin="round" stroke-width="1" d="M12 8v4m0 4h.01M21 12a9 9 0 11-18 0 9 9 0 0118 0z"/>
        </svg>
        <span class="theme-text-muted text-lg">Circuit diagram not available</span>
        <p class="theme-text-muted text-sm mt-2">Run "Apply Matrices" first to generate the schematic</p>
        <p class="theme-text-muted text-xs mt-1">Looking for: ./build/schematic.svg</p>
      </div>
    `;
  }
}

function initModal() {
  const infoBox = document.getElementById('infoBox');
  const modalOverlay = document.getElementById('modalOverlay');
  const closeModal = document.getElementById('closeModal');

  if (infoBox && modalOverlay && closeModal) {
    // Open modal when info box is clicked
    infoBox.addEventListener('click', () => {
      modalOverlay.classList.remove('hidden');
      document.body.style.overflow = 'hidden'; // Prevent background scroll
      loadCircuitDiagram(); // Load SVG when modal opens

      // Hide exclamation mark when viewing circuit diagram
      hideExclamationMark();
    });

    // Close modal functions
    const closeModalFn = () => {
      modalOverlay.classList.add('hidden');
      document.body.style.overflow = 'auto';
      currentZoom = 1;
      // Exit fullscreen if active
      if (isFullscreen) {
        if (document.exitFullscreen) {
          document.exitFullscreen();
        } else if (document.webkitExitFullscreen) {
          document.webkitExitFullscreen();
        } else if (document.msExitFullscreen) {
          document.msExitFullscreen();
        }
        isFullscreen = false;
      }
    };

    closeModal.addEventListener('click', closeModalFn);
    modalOverlay.addEventListener('click', (e) => {
      if (e.target === modalOverlay) closeModalFn();
    });
    document.addEventListener('keydown', (e) => {
      if (e.key === 'Escape' && !modalOverlay.classList.contains('hidden')) {
        closeModalFn();
      }
    });
  }
}

function updateQFormatDisplay() {
  const qInt = document.getElementById("q_integer").value || "8";
  const qFrac = document.getElementById("q_fractional").value || "8";
  const formatText = document.getElementById("q-format-text");
  if (formatText) {
    formatText.textContent = `Fixed-point Q${qInt}.${qFrac}`;
  }
}

document.addEventListener("DOMContentLoaded", () => {
  const btn = document.getElementById("applyMatricesBtn");
  if (btn) btn.addEventListener("click", applyMatrices);

  const aCols = document.getElementById("matrixA_cols");
  const bRows = document.getElementById("matrixB_rows");

  aCols.addEventListener("input", (e) => {
    bRows.value = e.target.value;
  });
  bRows.addEventListener("input", (e) => {
    aCols.value = e.target.value;
  });

  // Update Q format display on input change
  const qInt = document.getElementById("q_integer");
  const qFrac = document.getElementById("q_fractional");
  if (qInt && qFrac) {
    qInt.addEventListener("input", updateQFormatDisplay);
    qFrac.addEventListener("input", updateQFormatDisplay);
  }

  initToggle();
  initModal();
});

function getInputValue(id) {
  const el = document.getElementById(id);
  if (!el) return null;
  const v = parseInt(el.value, 10);
  return Number.isInteger(v) && v > 0 ? v : null;
}

function markValidity(id, ok) {
  const el = document.getElementById(id);
  if (!el) return;
  el.classList.remove(
    "ring-2","ring-neutral-400","border-neutral-400",
    "ring-red-500","border-red-500","ring-red-400","border-red-400",
    "ring-red-300","border-red-300","bg-red-50"
  );
  if (!ok) {
    el.classList.remove("border-neutral-300");
    el.classList.add("ring-2","ring-red-300","border-red-300","bg-red-50");
  } else if (!el.classList.contains("border-neutral-300")) {
    el.classList.add("border-neutral-300");
  }
}

function initToggle() {
  const toggle = document.getElementById('toggle');
  if (!toggle) return;

  // Apply stored theme
  const stored = localStorage.getItem('theme');
  const isDark = stored === 'dark';
  if (isDark) {
    document.documentElement.classList.add('dark');
    toggle.checked = true;
  }

  toggle.addEventListener('change', (e) => {
    const nowDark = e.target.checked;
    document.documentElement.classList.toggle('dark', nowDark);
    localStorage.setItem('theme', nowDark ? 'dark' : 'light');
  });
}
