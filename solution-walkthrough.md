# explanations

## questions

### 7.1 merge 2 sorted list

> purpose of the problem
list 1: 2 -> 5 -> 7
list 2: 3 -> 11
final: 2 -> 3 -> 5 -> 7 ->11

do it in-place  meaning: O(1)

tail=sentinal_node{};
append(node,tail)

// crux of appending and provide node to append via a condition check
tail->next = &node;
tail=node;
node=node->next;

> [!NOTE]
> for it's variant problem of merging 2 sorted doubly linked lists try changing append function.

### 7.2 reverse a sublist

List: 11 -> 3 -> 5 -> 7 -> 2

s = 2
f = 4

(both inclusive)

3 -> 5 -> 7

c->next = c->next->next;
reverse 3 nodes at a time.
