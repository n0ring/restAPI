#!/bin/bash


echo "Test with id 1"
curl -X 'GET' \
  'http://localhost/nodes/1' \
  -H 'accept: application/json'
echo "end of test"
echo 


echo "Test with id 2"
curl -X 'GET' \
  'http://localhost/nodes/2' \
  -H 'accept: application/json'
echo "end of test"
echo 


echo "Test with id 3"
curl -X 'GET' \
  'http://localhost/nodes/3' \
  -H 'accept: application/json'
echo "end of test"
echo 

echo "Test with id 4"
curl -X 'GET' \
  'http://localhost/nodes/4' \
  -H 'accept: application/json'
echo "end of test"
echo 


echo "Test to ubuntu id 4"
curl -X 'GET' \
  'https://triangle-1958.usr.yandex-academy.ru/nodes/4' \
  -H 'accept: application/json'
echo "end of test"
echo 