class Rule:
    def __init__(self, not_term: str, right_part: str):
        self.not_term = not_term
        self.right_part = right_part

    def __eq__(self, other) -> bool:
        return ((self.not_term == other.not_term) and
                (self.right_part == other.right_part))

    def __hash__(self) -> int:
        return hash((self.not_term, self.right_part))

class Grammar:
    def __init__(self, not_terms: set, alphabet: set, start: str):
        self.not_terms = not_terms
        self.alphabet = alphabet
        self.rules = set()
        self.start = start

    def _check_right_part(self, right_part: str) -> bool:
        for symbol in right_part:
            if symbol not in self.alphabet and symbol not in self.not_terms:
                return True

        return False

    def add_rule(self, rule: Rule) -> None:
        if rule.not_term not in self.not_terms:
            raise Exception('Not correct left part in rule: ' + rule.not_term + '->' + rule.right_part)
        elif self._check_right_part(rule.right_part):
            raise Exception('Not correct right part in rule: ' + rule.not_term + '->' + rule.right_part)
        else:
            self.rules.add(rule)

    def not_correct_word(self, word: str) -> bool:
        for symbol in word:
            if symbol not in self.alphabet:
                return True

        return False