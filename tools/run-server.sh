#!/bin/sh

cd ../cleaner_server
source /Users/linearlys/Library/Caches/pypoetry/virtualenvs/cleaner-JHqlw0ln-py3.12/bin/activate
uvicorn cleaner.main:app --host 0.0.0.0

