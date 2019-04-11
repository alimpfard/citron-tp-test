let $c_{rows}$ be the number of rows in a given keyboard layout, and $c_{cols}$ be the number of columns.

define $x_A$ and $y_A$ as $x_A = {({{c_{rows} + 1} \over {2}})}$ and $y_A = {({{c_{cols} + 1} \over {2}})}$.

As such, for all keyboard layouts, the probability of correctness function $P_c$ can be defined

as        $P_c(i_0, i_1, j_0, j_1) = 2 \cdot thresh_1 - C_t \cdot (abs({i_1 - i_0 + offset_{i_0} - offset_{i_1}}) + abs({j_1 - j_0}))$

where
        $C_t = {{thresh_1 \cdot m_{max}} \over {m_{min}} \cdot (x_A + y_A - 2)}$

and
        $offset_r = {{cRows_{max} - cRows_{r}} \over 2}$.

where $thresh_1$ is a parameter that gives the half-way threshold and its scale, $m_{max}$ and $m_{min}$ are the maximum and minimum number of mistakes allowed in a single word.

### Filtering the word-tree
if the word exists in the tree, per char in input word and active char in current subtree,
$thresh_{node} = thresh_{parent(node)} \cdot thresh_{value}$

if $thresh_{node}$ < $thresh_1$, disable the entire subtree.
