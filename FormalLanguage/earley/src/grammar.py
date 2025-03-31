class Grammar:
    def __init__(self, not_terms: set, alphabet: set, start: str):
        self.not_terms = not_terms
        self.not_terms.add('$')
        self.alphabet = alphabet
        self.start = '$'

        self.rules = dict()
        for symbol in not_terms:
            self.rules[symbol] = list()
            if symbol == '$':
                self.rules[symbol].append(start)

    def _check_right_part(self, right_part: str) -> bool:
        for symbol in right_part:
            if symbol not in self.alphabet and symbol not in self.not_terms:
                return True

        return False

    def add_rule(self, left_part: str, right_part: str) -> None:
        if left_part not in self.rules:
            raise Exception('Not correct left part in rule: ' + left_part + '->' + right_part)
        elif self._check_right_part(right_part):
            raise Exception('Not correct right part in rule: ' + left_part + '->' + right_part)
        else:
            self.rules[left_part].append(right_part)

    def not_correct_word(self, word: str) -> bool:
        for symbol in word:
            if symbol not in self.alphabet:
                return True

        return False