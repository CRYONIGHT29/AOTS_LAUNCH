import time
import tempfile
import json
from geopy.distance import geodesic
import firebase_admin
from firebase_admin import credentials, db
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient

# === AWS IoT Certificate Strings ==
ROOT_CA = """-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----"""


PRIVATE_KEY = """-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEArp7ej8mQGF605+HrrESm1MPdrOxD8H1NpysWHExM+bLMEjnn
IDVCbYchRh6K/fHYQjq0AVhCXqclPqxdX1iKVvHS5pes8jPb//Y0tzkGKtr+I+BT
UvWi1GOmpBMYQNt3M+dA1aVgVeGDE+L8mq/VC4BIzgbppr9pAu+oxRE1WqrdANK5
5l/bFcUq8jVXMS6tgSrjzM3L7jpGl1LCktpNw7B1jKUvSjWuDb6fLwtdgNuva7Gy
1Nwjr9h3q2mBw3a+NCvxm1UzMyxdga18hhhRevblBfnM+iFh9rYGMsXrmAmA4qH8
yP3u5XSsNWLnfwRCdiknQI+w/lM0zeWhFrxOtwIDAQABAoIBAQCG4KWuUJVhjX2U
gnekF175htSp5dDQaCjbz7cDYpguYsGn7lutEBZgPXkhDSR05CITff5d74tMSzTE
hSSWKfCpGZ66iDuI1S/ANK8hOwWdgou8sGAL+zmrbdFgeHNKXXpkd+InY9bWXxr8
+F3/KYOYn50crlA5gS2em/kzRoh8n6fz0RO4qtEVGBteNbaU5Dfj9q5tG1+qzj6I
zEdqutZQai0mrlQzbN1e/2mNDkZU/owfgMZCzTSY5XxmlFZY7FsgeLWVP8YYdqPO
LWTNDnn+NvTI5jSeN6WFER7yeLgLnqeUX6CUFKGksZPYdgiL8nZ/Pmq1IzYvIyLk
fAvGRytZAoGBAOTB1DVruUfKukBsUQa8PjeApGPzNupHgjjvLVc/bCRhEcHWCkKo
Uor4m6Jr4b/h4o4TyDG/zBcX5n4wyDadxG+uDiL1sU9Uw+mcCZhBNdqzW6GKHulG
YGKB+k+56JtXK4dHAaMQFKNS0KnyXJfT5ii+NPcV/myY9iphibpWOOMjAoGBAMNq
kRo74MzQZle7PqrGb9pHt7V+Ha6WgEFygWiPQCLN92b56fgEcQHl4iV3HlTGn0S5
JvxVS/hF8pCROhY3pqoySg6ubs7gDEJAY/VzfYMZwC0Q5ox5PvVop0gbKA5fJWBh
RoUViaj7hpF2BFQoyJyKGH+/m7lL0rDnH2Fz9TldAoGAL+AxHY0SFehj2p/QxWwY
SV1vCkwYrieRJYFJJ/1qEKMQKFU3iIPZ4qPtTqztbk9ELyAMwfy2Jg8mejqTbHrQ
Zd8zrjVliojfB8FUcL0VHFR1VPWnSoMkIPm6/XyXJgu0XHeL7CitaREoAqlqn4wF
m3lTdHLx8KCCxJPtU0ZAWakCgYA9uP8z5g3PI1PwCfh1QFA8BcME1FzTtD22blBp
kjWTyLKIGA/Kgrye+NlBNCqkZEgSENk7gkWIma9P8k8CjXla8J9rm7TsKuA0XNo1
wNC4A/4rPddUL0evg+qyX8F7KJpJou//QRpGAYwzja84VdlneEbcssyg7k4SCi5j
PZkLEQKBgGzcIcgh9i9rsXn/yTH4nZWrRPCGk+eRBytCdmtR4BRcC+yKGUk6tWv6
U8noFP+pq8+ZR7P2BX67q/YLfubrmUuoItq0/XD9lKJSeAa3Gw3HqCfUax/gFFxM
e9iqVzWaiBpV67hrYgI8b+bazo5PdW9MCgk8W6GRE4EAZupK1xTj
-----END RSA PRIVATE KEY-----"""


CERTIFICATE = """-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUcGo/B+whe0/HIENhI68FnULgfQIwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI1MDIxMTEwMzYx
MFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAK6e3o/JkBhetOfh66xE
ptTD3azsQ/B9TacrFhxMTPmyzBI55yA1Qm2HIUYeiv3x2EI6tAFYQl6nJT6sXV9Y
ilbx0uaXrPIz2//2NLc5Bira/iPgU1L1otRjpqQTGEDbdzPnQNWlYFXhgxPi/Jqv
1QuASM4G6aa/aQLvqMURNVqq3QDSueZf2xXFKvI1VzEurYEq48zNy+46RpdSwpLa
TcOwdYylL0o1rg2+ny8LXYDbr2uxstTcI6/Yd6tpgcN2vjQr8ZtVMzMsXYGtfIYY
UXr25QX5zPohYfa2BjLF65gJgOKh/Mj97uV0rDVi538EQnYpJ0CPsP5TNM3loRa8
TrcCAwEAAaNgMF4wHwYDVR0jBBgwFoAU2qbWpaGqb7+IYmo23R9EJjNhiDMwHQYD
VR0OBBYEFEiWqdoesN5TzCwaRuTaJBHoqBfKMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQCdatDuw1YUKEkosYFxla6Yf8iS
ooblIi1Vd/t8aHPkpZJ9G/Q4LPJNgtnP9Hv0C/L+uRsh3BJIwXQZtd3fJL9FpXfQ
nSnKGpzjCaoePjWjgpH5zhrtkXdWmHUKrpYqjlKN61aBkPq/2z4KKV8QYGoaREih
1QsodhUKqPyvTM9b2z8C1VEDfLEHD5t4m1PAnqK7RHk2LmfxAYajKeErg0Ms28gK
t1+Gp4ssD6stsXtPNgGH66f2YcHMXJVDH4u2jhzxCUabb7h01T/EFxTkMlAvXZYv
LGIBdWv8uRPGx1xE3hPuY1PMN2uON3rwTVrR8En/PBjPRSQj0NZKqS9gSmoY
-----END CERTIFICATE-----"""

# === Write certs to temp files ===
def write_temp_cert(name, content):
    file = tempfile.NamedTemporaryFile(delete=False, mode='w', suffix=name)
    file.write(content)
    file.close()
    return file.name

ca_path = write_temp_cert("ca.pem", ROOT_CA)
key_path = write_temp_cert("private.key", PRIVATE_KEY)
cert_path = write_temp_cert("cert.pem", CERTIFICATE)

# === MQTT Setup with Retry ===
client = AWSIoTMQTTClient("AmbulanceClient")
client.configureEndpoint("a1jsymzdtk2uy7-ats.iot.ap-south-1.amazonaws.com", 8883)
client.configureCredentials(ca_path, key_path, cert_path)
client.configureOfflinePublishQueueing(-1)
client.configureDrainingFrequency(2)
client.configureConnectDisconnectTimeout(10)
client.configureMQTTOperationTimeout(5)

def connect_with_retry(client, max_retries=5, base_delay=2):
    for attempt in range(1, max_retries + 1):
        try:
            client.connect()
            print("✅ MQTT connected")
            return True
        except Exception as e:
            print(f"❌ MQTT connection attempt {attempt} failed: {e}")
            time.sleep(base_delay * attempt)
    print("❌ Failed to connect to MQTT after retries.")
    return False

# === Firebase Initialization ===
if not firebase_admin._apps:
    firebase_cred = credentials.Certificate("serviceAccountKey.json")
    firebase_admin.initialize_app(firebase_cred, {
        'databaseURL': 'https://aots-13497-default-rtdb.firebaseio.com/'
    })
    print("✅ Firebase initialized")

def get_latest_ambulance_data(retries=3):
    for attempt in range(retries):
        try:
            ref = db.reference("ambulances/nishad").order_by_child("timestamp").limit_to_last(1)
            return ref.get()
        except Exception as e:
            print(f"⚠️ Firebase fetch failed (attempt {attempt + 1}): {e}")
            time.sleep(2)
    return None

# === Coordinates ===
intersection_center = (15.593728, 73.814221)
lanes = {
    "lane1": (15.594703, 73.814801),
    "lane2": (15.593485, 73.815439),
    "lane3": (15.592731, 73.813374),
    "lane4": (15.594679, 73.813201),
}

previous_center_distance = None
active_lane = None
lane_high_sent = False

# === Try connecting MQTT ===
if not connect_with_retry(client):
    exit(1)

# === Main Loop ===
while True:
    try:
        data = get_latest_ambulance_data()

        if data:
            latest_entry = list(data.values())[0]
            lat = float(latest_entry.get("latitude"))
            lon = float(latest_entry.get("longitude"))
            position = (lat, lon)

            center_distance = geodesic(position, intersection_center).meters
            is_approaching_center = (
                previous_center_distance is None or center_distance < previous_center_distance
            )
            previous_center_distance = center_distance

            # Find nearest lane (but only if no lane is currently active)
            closest_lane = None
            min_distance = float("inf")
            if not lane_high_sent:  # only detect lane if nothing is active
                for lane_name, coord in lanes.items():
                    dist = geodesic(position, coord).meters
                    if dist <= 200 and dist < min_distance:
                        closest_lane = lane_name
                        min_distance = dist

            # === Send HIGH if approaching and within lane zone ===
            if closest_lane and is_approaching_center and not lane_high_sent:
                client.publish("gps/ping", f"{closest_lane} high", 1)
                print(f"📶 {closest_lane} HIGH sent")
                active_lane = closest_lane
                lane_high_sent = True

            # === Send LOW after passing center ===
            elif not is_approaching_center and center_distance > 20 and lane_high_sent:
                if active_lane:
                    client.publish("gps/ping", f"{active_lane} low", 1)
                    print(f"📴 {active_lane} LOW sent")
                    lane_high_sent = False
                    active_lane = None

        else:
            print("⚠️ No ambulance data found.")

    except Exception as e:
        print(f"❌ Main loop error: {e}")

    time.sleep(5)
