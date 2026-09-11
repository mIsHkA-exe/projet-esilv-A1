
        function showToast(msg) {
            const t = document.getElementById('toast');
            t.textContent = msg;
            t.classList.add('show');
            setTimeout(() => t.classList.remove('show'), 2800);
        }

        function handleAccept() {
            document.getElementById('toggle-audience').checked = true;
            document.getElementById('toggle-security').checked = true;
            showToast('✦ Tous les cookies ont été acceptés');
        }

        function handleRefuse() {
            document.getElementById('toggle-audience').checked = false;
            document.getElementById('toggle-security').checked = false;
            document.getElementById('customize-panel').classList.remove('open');
            document.getElementById('btn-customize').classList.remove('active');
            showToast('Cookies optionnels refusés');
        }

        function togglePanel() {
            const panel = document.getElementById('customize-panel');
            const btn   = document.getElementById('btn-customize');
            panel.classList.toggle('open');
            btn.classList.toggle('active');
        }

        function handleSave() {
            const audience = document.getElementById('toggle-audience').checked;
            const security = document.getElementById('toggle-security').checked;
            const parts = ['Techniques (activés)'];
            if (audience) parts.push('Audience');
            if (security) parts.push('Sécurité');
            showToast('✦ Préférences enregistrées : ' + parts.join(', '));
        }