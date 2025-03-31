import pytest
from src.earley import Grammar
from src.earley import Earley

@pytest.fixture()
def get_grammar(not_terms: set, alphabet: set, rules: list, start: str):
    grammar = Grammar(not_terms, alphabet, start)
    for rule in rules:
        grammar.add_rule(rule[0], rule[1])

    return grammar

@pytest.mark.parametrize('not_terms', [{'S'}])
@pytest.mark.parametrize('alphabet', [{'(', ')'}])
@pytest.mark.parametrize('rules', [[('S', '(S)S'), ('S', '')]])
@pytest.mark.parametrize('start', ['S'])
def test_psp1(get_grammar):
    earley = Earley()
    earley.fit(get_grammar)

    assert earley.predict('((') == False
    assert earley.predict('())') == False
    assert earley.predict('(((())))') == True
    assert earley.predict('()') == True
    assert earley.predict('()()') == True
    assert earley.predict('') == True
    assert earley.predict('(()())()') == True
    assert earley.predict('(()()())(()())') == True
    assert earley.predict('(()()())(()()') == False

@pytest.mark.parametrize('not_terms', [{'S'}])
@pytest.mark.parametrize('alphabet', [{'(', ')'}])
@pytest.mark.parametrize('rules', [[('S', '(S)'), ('S', ''), ('S', 'SS')]])
@pytest.mark.parametrize('start', ['S'])
def test_psp2(get_grammar):
    earley = Earley()
    earley.fit(get_grammar)

    assert earley.predict('((') == False
    assert earley.predict('())') == False
    assert earley.predict('(((())))') == True
    assert earley.predict('()') == True
    assert earley.predict('()()') == True
    assert earley.predict('') == True
    assert earley.predict('(()())()') == True
    assert earley.predict('(()()())(()())') == True
    assert earley.predict('(()()())(()()') == False

@pytest.mark.parametrize('not_terms', [{'S'}])
@pytest.mark.parametrize('alphabet', [{'m', 'a'}])
@pytest.mark.parametrize('rules', [[('S', 'mama')]])
@pytest.mark.parametrize('start', ['S'])
def test_mama(get_grammar):
    earley = Earley()
    earley.fit(get_grammar)

    assert earley.predict('mama') == True
    assert earley.predict('amma') == False
    assert earley.predict('') == False
    assert earley.predict('am') == False

@pytest.mark.parametrize('not_terms', [{'S'}])
@pytest.mark.parametrize('alphabet', [{'a', 'b'}])
@pytest.mark.parametrize('rules', [[('S', 'aSbS'), ('S', 'bSaS'), ('S', '')]])
@pytest.mark.parametrize('start', ['S'])
def test_grammar1(get_grammar):
    earley = Earley()
    earley.fit(get_grammar)

    assert earley.predict('aa') == False
    assert earley.predict('ab') == True
    assert earley.predict('abababab') == True
    assert earley.predict('abbaabbab') == False
    assert earley.predict('aaabbb') == True
    assert earley.predict('ababb') == False
    assert earley.predict('') == True
    assert earley.predict('aabbaaabbb') == True
    assert earley.predict('ababbabbabbab') == False

@pytest.mark.parametrize('not_terms', [{'S', 'T'}])
@pytest.mark.parametrize('alphabet', [{'a', 'b'}])
@pytest.mark.parametrize('rules', [[('S', 'aTa'), ('S', 'bb'), ('T', ''), ('T', 'abT')]])
@pytest.mark.parametrize('start', ['S'])
def test_grammar2(get_grammar):
    earley = Earley()
    earley.fit(get_grammar)

    assert earley.predict('aa') == True
    assert earley.predict('bb') == True
    assert earley.predict('aaba') == True
    assert earley.predict('aba') == False
    assert earley.predict('aaabbb') == False
    assert earley.predict('aababa') == True