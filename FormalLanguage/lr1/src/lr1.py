from copy import deepcopy as copy
from src.grammar import Rule, Grammar

START = '#'
END = '$'

class Configuration:
    def __init__(self, rule: Rule, next_symbol: str, point_position: int):
        self.rule = rule
        self.next_symbol = next_symbol
        self.point_position = point_position

    def __eq__(self, other) -> bool:
        return ((self.rule == other.rule) and (self.next_symbol == other.next_symbol) and
                (self.point_position == other.point_position))

    def __hash__(self) -> int:
        return hash((self.rule, self.next_symbol, self.point_position))

class Node:
    def __init__(self):
        self.children = {}
        self.confs = set()

    def __eq__(self, other) -> bool:
        return self.confs == other.confs

    def __hash__(self) -> int:
        return hash(tuple(self.confs))

class Shift:
    def __init__(self, to: int):
        self.to = to

class Reduce:
    def __init__(self, rule: Rule):
        self.rule = rule

class LR1:
    def __init__(self):
        self.grammar = None
        self.nodes = None
        self.nodes_set = None
        self.table = None

    def _add_not_term(self, alpha: str, result: list, result_set: set) -> bool:
        is_update = False
        for rule in self.grammar.rules:
            if rule.not_term != alpha:
                continue

            if alpha in result_set:
                is_update = True
                result_set.discard(alpha)

            if ((alpha != rule.right_part[:1]) and
                    (rule.right_part[:1] not in result_set)):
                is_update = True
                result_set.add(rule.right_part[:1])
                result.append(rule.right_part[:1])

        return is_update

    def _add_new(self, word: str, cur: set) -> set:
        if word in cur:
            return set()

        cur.add(word)
        if len(word) == 0:
            return set()

        result = [word[0]]
        result_set = {word[0]}
        if word in self.grammar.alphabet:
            return result_set

        changed = True
        while changed:
            changed = False
            index = 0
            while index < len(result):
                alpha = result[index]
                if alpha in self.grammar.alphabet:
                    break

                changed = changed or self._add_not_term(alpha, result, result_set)
                index += 1

        if '' in result_set:
            result_set.remove('')
            result_set.update(self._add_new(word[1:], cur))
        return result_set

    def _closure(self, node: Node) -> Node:
        is_update = True
        while is_update:
            new_node = copy(node)
            is_update = False
            for conf in node.confs:
                for rule in self.grammar.rules:
                    if ((len(conf.rule.right_part) > conf.point_position) and
                            (conf.rule.right_part[conf.point_position] == rule.not_term)):
                        for next_symbol in self._add_new(conf.rule.right_part[conf.point_position + 1:] + conf.next_symbol, set()):
                            if Configuration(rule, next_symbol, 0) not in new_node.confs:
                                new_node.confs.add(
                                    Configuration(rule, next_symbol, 0))
                                is_update = True
            node = new_node

        return node

    def _add(self, i: int, char: str) -> None:
        new_node = Node()
        for conf in self.nodes[i].confs:
            if ((len(conf.rule.right_part) > conf.point_position) and
                    (conf.rule.right_part[conf.point_position] == char)):
                new_node.confs.add(Configuration(conf.rule, conf.next_symbol, conf.point_position + 1))

        new_node = self._closure(new_node)
        if new_node not in self.nodes_set:
            self.nodes.append(new_node)
            self.nodes_set.add(new_node)

        if char in self.nodes[i].children:
            raise Exception('Not LR(1) grammar')

        self.nodes[i].children[char] = self.nodes.index(new_node)

    def _fill(self, i: int, used: set) -> None:
        if i in used:
            return

        for symbol in self.nodes[i].children:
            self.table[i][symbol] = Shift(self.nodes[i].children[symbol])

        for conf in self.nodes[i].confs:
            if len(conf.rule.right_part) == conf.point_position:
                if conf.next_symbol in self.table[i]:
                    raise Exception('Not LR(1) grammar')
                self.table[i][conf.next_symbol] = Reduce(conf.rule)

        used.add(i)
        for symbol in self.nodes[i].children:
            self._fill(self.nodes[i].children[symbol], used)

    def fit(self, grammar: Grammar) -> None:
        self.grammar = grammar
        self.nodes = [Node()]
        self.nodes[0].confs.add(Configuration(Rule(START, grammar.start), END, 0))
        self.nodes[0] = self._closure(self.nodes[0])
        self.nodes_set = {self.nodes[0]}

        i = 0
        while i < len(self.nodes):
            processed = set()
            for conf in self.nodes[i].confs:
                if ((len(conf.rule.right_part) > conf.point_position) and
                        (conf.rule.right_part[conf.point_position] not in processed)):
                    self._add(i, conf.rule.right_part[conf.point_position])
                    processed.add(conf.rule.right_part[conf.point_position])
            i += 1

        self.table = [{} for _ in range(len(self.nodes))]
        self._fill(0, set())

    def predict(self, word: str) -> bool:
        word += END
        stack = [0]
        i = 0

        while i < len(word):
            alpha = word[i]
            stack_back = stack[-1]
            if alpha not in self.table[stack_back]:
                return False

            if isinstance(self.table[stack_back][alpha], Reduce):
                if self.table[stack_back][alpha].rule == Rule(START, self.grammar.start):
                    if i == (len(word) - 1):
                        return True
                    return False
                if (len(self.table[stack_back][alpha].rule.right_part) * 2) >= len(stack):
                    return False

                next_stack_elem = self.table[stack_back][alpha].rule.not_term
                rule_len = len(self.table[stack_back][alpha].rule.right_part)

                stack = stack[:len(stack) - (rule_len * 2)]
                stack_back = stack[-1]
                stack.append(next_stack_elem)
                stack.append(self.table[stack_back][next_stack_elem].to)
            elif isinstance(self.table[stack_back][alpha], Shift):
                stack.append(alpha)
                stack.append(self.table[stack_back][alpha].to)
                i += 1
        return False