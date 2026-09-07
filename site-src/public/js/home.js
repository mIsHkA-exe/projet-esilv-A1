document.addEventListener('DOMContentLoaded', function () {
    const form = document.getElementById('booking-form');
    if (!form) return;

    const btn = document.getElementById('btn-check');
    
    // Select all required inputs
    const inputs = form.querySelectorAll('input[required], select[required]');

    // French plate regex
    const PLATE_REGEX = /^[A-Za-z]{2}-\d{3}-[A-Za-z]{2}$/;

    // 1. Function to check if the whole form is okay
    function checkFormValidity() {
        let allValid = true;

        inputs.forEach(input => {
            if (input.name === 'plate') {
                if (!PLATE_REGEX.test(input.value.trim())) allValid = false;
            } else if (input.id === 'date-end') {
                const start = document.getElementById('date-start').value;
                if (!input.value || input.value < start) allValid = false;
            } else {
                if (!input.value.trim()) allValid = false;
            }
        });

        // Enable or disable button
        btn.disabled = !allValid;
    }

    // 2. Function to highlight a single field
    function validateField(field) {
        let isValid = false;
        const val = field.value.trim();

        if (field.name === 'plate') {
            isValid = PLATE_REGEX.test(val);
        } else if (field.id === 'date-end') {
            const start = document.getElementById('date-start').value;
            isValid = val !== '' && (start === '' || val >= start);
        } else {
            isValid = val !== '';
        }

        if (isValid) {
            field.classList.remove('field-invalid');
            field.classList.add('field-valid');
        } else {
            field.classList.add('field-invalid');
            field.classList.remove('field-valid');
        }
    }

    // 3. Listen for changes in real-time
    inputs.forEach(input => {
        // We use 'input' so it checks on every key press
        input.addEventListener('input', () => {
            validateField(input);
            checkFormValidity();
        });

        // Also check when the user leaves the field (blur)
        input.addEventListener('blur', () => {
            validateField(input);
        });
    });

    // Run once at start to make sure button is disabled
    checkFormValidity();
    /* ─────────────────────────────────────────────────────────
   SOUMISSION (public/js/script.js)
   ───────────────────────────────────────────────────────── */
form.addEventListener('submit', function (e) {
    e.preventDefault(); // Empêche la page de se recharger

    if (!isFormValid()) {
        // Si le formulaire est invalide, on affiche les erreurs
        validateField(dateStart, true);
        validateField(dateEnd, true);
        // ... etc
        return;
    }

    // SI TOUT EST VALIDE : On redirige
    // On récupère l'URL stockée dans l'attribut data-href du bouton
    const targetUrl = btn.getAttribute('data-href');
    
    // Animation optionnelle : on change le texte du bouton
    btn.textContent = "Loading Parking Map...";
    btn.style.opacity = "0.7";

    // Redirection après un mini délai pour l'effet visuel
    setTimeout(() => {
        window.location.href = targetUrl;
    }, 500);
});
});