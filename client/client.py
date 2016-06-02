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
    for re in r.json()['result']:
        print (re['domain'] + " => " + re['status'])

def remove_domains(domains_table):
    payload = {}
    payload["command"] = "remove"
    payload["domains"] = domains_table
    r = requests.post("http://localhost:8080", json=payload)
    for re in r.json()['result']:
        print (re['domain'] + " => " + re['status'])

def query_domains(domains_table):
    payload = {}
    payload["command"] = "query"
    if domains_table:
        payload["domains"] = domains_table
    else:
        payload["domains"] = "*"
    r = requests.post("http://localhost:8080", json=payload)
    if r.json()['result']:
        for re in r.json()['result']:
            print (re['domain'] + " => " + re['status'])

def query_test(domains_table):
    payload = {}
    payload["com"] = "ry"
    payload["mains"] = domains_table
    r = requests.post("http://localhost:8080", json=payload)
    print (r.headers)

parser = argparse.ArgumentParser()
parser.add_argument("command", choices=("add", "remove", "query", "test"))
parser.add_argument('domain', nargs='*')
args = parser.parse_args()
domains_table = [x for x in args.domain]
if args.command == "add" and domains_table:
    add_domains(domains_table)
elif args.command == "remove" and domains_table:
    remove_domains(domains_table)
elif args.command == "query":
    query_domains(domains_table)
elif args.command == "test":
    query_test(domains_table)
else:
    parser.print_help()
