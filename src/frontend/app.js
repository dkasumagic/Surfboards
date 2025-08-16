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

  try {
    if (typeof webui !== "undefined" && typeof webui.call === "function") {
      const result = await webui.call("inputMatrix", r1, c1, r2, c2);
      console.log("result:", result);
    } else {
      console.warn("webui.call is not available.");
    }
  } catch (err) {
    console.error("inputMatrix call failed", err);
  } finally {
    stopGenerating(applyBtn);
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
