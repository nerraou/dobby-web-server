<pre>
<?php

$fields = [
  "GATEWAY_INTERFACE",
  "SERVER_SOFTWARE"  ,
  "QUERY_STRING"     ,
  "REQUEST_METHOD"   ,
  "CONTENT_TYPE"     ,
  "CONTENT_LENGTH"   ,
  "SCRIPT_FILENAME"  ,
  "SCRIPT_NAME"      ,
  "REQUEST_URI"      ,
  "DOCUMENT_URI"     ,
  "DOCUMENT_ROOT"    ,
  "SERVER_PROTOCOL"  ,
  "REMOTE_ADDR"      ,
  "REMOTE_PORT"      ,
  "REMOTE_HOST"      ,
  "SERVER_ADDR"      ,
  "SERVER_PORT"      ,
  "SERVER_NAME"      ,
  "PATH_TRANSLATED"  ,
  "PATH_INFO"        ,
];

foreach ($fields as $fieldName) {
    echo $fieldName . ': ' . $_SERVER[$fieldName] . PHP_EOL;
}

echo '--------------------' . PHP_EOL;

var_dump($_POST);

var_dump($_FILES);
?>
</pre>
