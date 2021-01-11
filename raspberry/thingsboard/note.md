
# Useful commands
# systemctl status thingsboard

SmartAPP
export APP_ID="7e45db6a-9190-4923-8ce4-4f9f344a6665"
Client ID:
    a39ff76e-3958-41b0-8fca-3dbd450c8223
Client Secret
    813e9e24-ca72-434a-aecc-903a302e8a20

curl --request PUT \
--url https://4adc81416f2f.ngrok.io/apps/630689d2-5e85-43c4-b450-cb3be6d804c1/signature-type \
--header "Accept: application/json" \
--header "Content-Type: application/json" \
--header "Authorization: Bearer ${PAT_TOKEN}" \
--data '{ "signatureType": "ST_PADLOCK" }' curl --request PUT \
--url https://4adc81416f2f.ngrok.io/apps/$630689d2-5e85-43c4-b450-cb3be6d804c1/signature-type \
--header "Accept: application/json" \
--header "Content-Type: application/json" \
--header "Authorization: Bearer ${PAT_TOKEN}" \
--data '{ "signatureType": "ST_PADLOCK" }' 


SmartThings Full Token
eb9d4f2f-284b-43f2-94ff-c317bc751b82

> 
curl --request GET --url https://api.smartthings.com/apps/${APP_ID}/confirmationUrl \
    --header "Authorization: Bearer eb9d4f2f-284b-43f2-94ff-c317bc751b82"

curl --request PUT \
    --url https://api.smartthings.com/apps/${APP_ID}/register \
    --header "Accept: application/json" \
    --header "Authorization: Bearer eb9d4f2f-284b-43f2-94ff-c317bc751b82" \
    --data '{"lifecycle": "CONFIRMATION", "confirmationData": { "appId": "a39ff76e-3958-41b0-8fca-3dbd450c8223", "confirmationUrl": "https://4adc81416f2f.ngrok.io/" } } '
1a1d87b8-7e0c-4ae9-9636-f59aefe5c010 38cba002-c0a3-47e1-ad39-388e50bd7ac1