document.getElementById('ipForm').addEventListener('submit', function(event) {
    event.preventDefault();
    const ip = document.getElementById('ip').value;
    fetch('/set-ip', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({ ip: ip })
    })
    .then(response => response.json())
    .then(data => {
        document.getElementById('info').innerText = `Connected to: ${data.baseUrl}`;
    });
});

let fetching = false;

function fetchData(topic) {
    fetching = true;
    fetchInterval = setInterval(() => {
        fetch(`/get-data?topic=${topic}`)
        .then(response => response.json())
        .then(data => {
            if (fetching) {
                document.getElementById(`data-${topic}`).innerText = JSON.stringify(data, null, 2);
            }
        })
        .catch(error => {
            console.error('Error fetching data:', error);
        });
    }, 2500); // Fetch data every 2.5 seconds
}

function stopFetching() {
    fetching = false;
}