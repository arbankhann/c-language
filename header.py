
# import requests

# url = "https://httpbin.org/headers"

# headers = {
#     "User-Agent": "Python-Header-Learning/1.0",
#     "Accept": "application/json",
#     "Accept-Language": "en-US,en;q=0.9"
# }

# response = requests.get(
#     url,
#     headers=headers,
#     timeout=15
# )

# print("Status Code:", response.status_code)
# print("\nRequest Headers:")
# print(response.request.headers)

# print("\nResponse Headers:")
# for name, value in response.headers.items():
#     print(f"{name}: {value}")

# print("\nServer Response:")
# print(response.text)





def my_num(n):
    if n>=11:
        return n
    else:
        print(n)
        my_num(n+1)
my_num(0)
