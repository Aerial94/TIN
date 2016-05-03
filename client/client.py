#!/usr/bin/env python
# -*- coding: utf-8 -*-

# DNS Checker example client

import requests
import json


def add_domain(domains_table):
    payload = {}
    payload["command"] = "add"
    payload["domains"] = domains_table
    return requests.post("http://localhost:1337", json=payload)


print(add_domain(['google.com','elka.pw.edu.pl']))
