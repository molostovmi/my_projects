import pytest
from src.lr1 import Grammar, LR1, Rule

@pytest.fixture()
def get_grammar(not_terms: set, alphabet: set, rules: list, start: str):
    grammar = Grammar(not_terms, alphabet, start)
    for rule in rules:
        grammar.add_rule(Rule(rule[0], rule[1]))

    return grammar

@pytest.mark.parametrize('not_terms', [{'S'}])
@pytest.mark.parametrize('alphabet', [{'(', ')'}])
@pytest.mark.parametrize('rules', [[('S', '(S)S'), ('S', '')]])
@pytest.mark.parametrize('start', ['S'])
def test_psp1(get_grammar):
    lr1 = LR1()
    lr1.fit(get_grammar)

    assert lr1.predict('((') == False
    assert lr1.predict('())') == False
    assert lr1.predict('(((())))') == True
    assert lr1.predict('()') == True
    assert lr1.predict('()()') == True
    assert lr1.predict('') == True
    assert lr1.predict('(()())()') == True
    assert lr1.predict('(()()())(()())') == True
    assert lr1.predict('(()()())(()()') == False

@pytest.mark.parametrize('not_terms', [{'S'}])
@pytest.mark.parametrize('alphabet', [{'m', 'a'}])
@pytest.mark.parametrize('rules', [[('S', 'mama')]])
@pytest.mark.parametrize('start', ['S'])
def test_mama(get_grammar):
    lr1 = LR1()
    lr1.fit(get_grammar)

    assert lr1.predict('mama') == True
    assert lr1.predict('amma') == False
    assert lr1.predict('') == False
    assert lr1.predict('am') == False