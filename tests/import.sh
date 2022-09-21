#/bin/bash

curl -X 'POST' \
  'http://localhost/imports' \
  -H 'accept: */*' \
  -H 'Content-Type: application/json' \
-d '{
  "items": [
    {
      "id": "42",
      "url": "/file/url1",
      "parentId": "2222",
      "size": 42,
      "type": "FILE"
    },
    {
      "id": "2222",
      "type": "FOLDER"
    }
  ],
  "updateDate": "2022-05-28T21:12:01.000Z"
}'

