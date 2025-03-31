from src.grammar import Grammar

class Situation:
    def __init__(self, left_part: str, right_part: str, dot: int, left: int):
        self.left_part = left_part
        self.right_part = right_part
        self.dot = dot
        self.left = left

    def __hash__(self):
        return (hash(self.left_part) + hash(self.right_part)) * hash(self.dot + self.left)

    def __eq__(self, other) -> bool:
        return (self.dot == other.dot and self.left == other.left and
                self.right_part == other.right_part and self.left_part == other.left_part)

class Earley:
    def __init__(self):
        self.grammar = None
        self.situations = list()

    def fit(self, grammar: Grammar) -> None:
        self.grammar = grammar

    def _init(self, word: str) -> None:
        self.situations.clear()
        for _ in range(len(word) + 1):
            self.situations.append(set())
        self.situations[0].add(Situation(self.grammar.start, self.grammar.rules[self.grammar.start][0], 0, 0))

    def _predict(self, ind: int) -> None:
        for sit in self.situations[ind].copy():
            if sit.dot < len(sit.right_part):
                left_part = sit.right_part[sit.dot]

                if left_part in self.grammar.not_terms:
                    for right_part in self.grammar.rules[left_part]:
                        self.situations[ind].add(Situation(left_part, right_part, 0, ind))

    def _scan(self, ind: int, word: str) -> None:
        for sit in self.situations[ind - 1]:
            if sit.dot < len(sit.right_part):
                if sit.right_part[sit.dot] == word[ind - 1]:
                    self.situations[ind].add(Situation(sit.left_part, sit.right_part, sit.dot + 1, sit.left))

    def _complete(self, ind: int) -> None:
        for sit in self.situations[ind].copy():
            if sit.dot == len(sit.right_part):
                for pred in self.situations[sit.left].copy():
                    if pred.dot < len(pred.right_part) and pred.right_part[pred.dot] == sit.left_part:
                        self.situations[ind].add(Situation(pred.left_part, pred.right_part, pred.dot + 1, pred.left))

    def _complete_predict(self, ind: int) -> None:
        old_size = len(self.situations[ind])
        self._complete(ind)
        self._predict(ind)
        while old_size != len(self.situations[ind]):
            old_size = len(self.situations[ind])
            self._complete(ind)
            self._predict(ind)

    def predict(self, word: str) -> bool :
        if self.grammar.not_correct_word(word):
            raise Exception('Not correct word: ' + word)

        self._init(word)
        self._complete_predict(0)

        for ind in range(1, len(word) + 1):
            self._scan(ind, word)
            self._complete_predict(ind)

        return Situation(self.grammar.start, self.grammar.rules[self.grammar.start][0], 1, 0) in self.situations[len(word)]