import requests

url = "https://example.com"

response = requests.get(url, timeout=10)

print("Status:", response.status_code)

print("\nResponse Headers:")
for key, value in response.headers.items():
    print(key, ":", value)