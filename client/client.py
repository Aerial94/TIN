#!/usr/bin/env python
# -*- coding: utf-8 -*-

# DNS Checker example client

import requests
import json
import argparse


def add_domains(domains_table):
    payload = {}
    payload["command"] = "add"
    payload["domains"] = domains_table
    r = requests.post("http://localhost:8080", json=payload)
    return r.json()

def remove_domains(domains_table):
    payload = {}
    payload["command"] = "remove"
    payload["domains"] = domains_table
    r = requests.post("http://localhost:8080", json=payload)
    return r.json()

def query_domains(domains_table):
    payload = {}
    payload["command"] = "query"
    payload["domains"] = domains_table
    r = requests.post("http://localhost:8080", json=payload)
    return r.json()

parser = argparse.ArgumentParser()
parser.add_argument("command", choices=("add", "remove", "query"))
parser.add_argument('domain', nargs='+')
args = parser.parse_args()
domains_table = [x for x in args.domain]
if args.command == "add":
    print(add_domains(domains_table))
elif args.command == "remove":
    print(remove_domains(domains_table))
elif args.command == "query":
    print(query_domains(domains_table))
