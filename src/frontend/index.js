async function applyMatrices() {
  const ids = ['matrixA_rows', 'matrixA_cols', 'matrixB_rows', 'matrixB_cols'];
  const values = ids.map(getInputValue);
  ids.forEach((id, i) => markValidity(id, values[i] !== null));

  console.log("applying matrixes")
  if (values.some(v => v === null)) {
    alert('Enter positive integer values for all matrix dimensions.');
    return;
  }

  const [r1, c1, r2, c2] = values;

  try {
    if (typeof webui !== 'undefined' && typeof webui.call === 'function') {
      const result = await webui.call('inputMatrix', r1, c1, r2, c2);
      console.log('Backend response:', result);
    } else {
      console.warn('webui.call is not available.');
    }
  } catch (err) {
    console.error('inputMatrix call failed', err);
    alert('Backend error while applying matrices.');
  }
}

document.addEventListener('DOMContentLoaded', () => {
  const btn = document.getElementById('applyMatricesBtn');
  console.log("loaded");
  if (btn) btn.addEventListener('click', applyMatrices);
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

  // Clear previous state (neutral or error)
  el.classList.remove(
    'ring-2',
    'ring-neutral-400',
    'border-neutral-400',
    'ring-red-500',
    'border-red-500',
    'ring-red-400',
    'border-red-400',
    'ring-red-300',
    'border-red-300',
    'bg-red-50'
  );

  if (!ok) {
    el.classList.remove('border-neutral-300');
    el.classList.add('ring-2', 'ring-red-300', 'border-red-300', 'bg-red-50');
  } else {
    // Restore default neutral border if missing
    if (!el.classList.contains('border-neutral-300')) {
      el.classList.add('border-neutral-300');
    }
  }
}