import json
import boto3
from decimal import Decimal  # 🔥 This is the key fix

dynamodb = boto3.resource('dynamodb')
table = dynamodb.Table('AmbulanceCoordinates')

def add_coordinates(body):
    lat = body.get("lat")
    lon = body.get("lon")
    timestamp = body.get("time")  # Ensure 'time' maps to 'timestamp'
    ambu_id = body.get("AmbuId")

    if lat is None or lon is None or timestamp is None or ambu_id is None:
        return 400, {"error": "Missing one or more required fields: lat, lon, time, AmbuId"}

    # Convert float to Decimal to avoid TypeError
    item = {
        "driverID": ambu_id,  # Ensure the partition key is correctly named 'driverID'
        "timestamp": timestamp,  # Ensure the sort key is correctly named 'timestamp'
        "lat": Decimal(str(lat)),  # Convert to Decimal
        "lon": Decimal(str(lon))   # Convert to Decimal
    }

    print(f"Item to be inserted: {item}")  # Debugging log to ensure the data is correct
    
    table.put_item(Item=item)
    return 200, {"message": "Coordinates saved successfully"}

def lambda_handler(event, context):
    path = event.get("path", "")
    method = event.get("httpMethod", "")
    body = json.loads(event.get("body", "{}")) if event.get("body") else {}

    print(f"Received {method} request for {path}")
    print(f"with body {body}")

    if path == "/ambulance-coordinates" and method == "POST":
        status, response = add_coordinates(body)
    else:
        status, response = 404, {"error": "code: Route not found"}

    return {
        "statusCode": status,
        "headers": {
            "Content-Type": "application/json"
        },
        "body": json.dumps(response)
    }
