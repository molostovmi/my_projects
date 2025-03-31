from src.lr1 import Grammar, LR1, Rule

_, _, count_of_rules = map(int, input().split())
not_terms = set(input())
alphabet = set(input())
alphabet.add('')

rules = list()
for i in range(count_of_rules):
    rule = input().replace(' ', '')

    if '->' not in rule:
        raise Exception('Not correct rule: ' + rule)

    left_part, right_part = rule.split('->')
    rules.append((left_part, right_part))

grammar = Grammar(not_terms, alphabet, input().strip())
for rule in rules:
    grammar.add_rule(Rule(rule[0], rule[1]))

earley = LR1()
earley.fit(grammar)

count_of_requests = int(input())
for i in range(count_of_requests):
    if earley.predict(input().strip()):
        print("Yes")
    else:
        print("No")