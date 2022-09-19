#/bin/bash

curl -X 'POST' \
  'http://localhost/imports' \
  -H 'accept: */*' \
  -H 'Content-Type: application/json' \
-d '{
  "items": [
    {
      "id": "1",
      "url": "/file/url1",
      "parentId": "2",
      "size": 234,
      "type": "FILE"
    },
    {
      "id": "2",
      "type": "FOLDER",
      "parentId": "4"
    }
  ],
  "updateDate": "2022-05-28T21:12:01.000Z"
}'

