<div>
<h1>MiniC Compiler – Full Frontend + IR + MIPS Backend</h1>

A compiler for a simplified C-like language written in C.

This project implements:

<ul>
  <li>Recursive descent parsing</li>
  <li>Abstract Syntax Tree (AST) construction</li>
  <li>Scoped symbol tables</li>
  <li>Semantic validation</li>
  <li>Three-Address Code (Quad) intermediate representation</li>
  <li>Short-circuit boolean code generation</li>
  <li>Stack-frame based MIPS assembly generation</li>
</ul>
</div>

<div>
<h2>Compiler Architecture:</h2>

```
Source Code
   ↓
Lexer
   ↓
Recursive Descent Parser
   ↓
AST Construction
   ↓
Semantic Analysis (Scoped Symbol Tables)
   ↓
Three-Address Code (Quad IR)
   ↓
MIPS Code Generation
```
</div>
<div>
<h2>Language Features</h2>
<h3>Supported:</h3>
<ul>
  <li>int variables</li>
  <li>Multiple declarations (int a, b, c;)</li>
  <li>Functions with parameters</li>
  <li>Nested scopes</li>
  <li>if / else</li>
  <li>while</li>
  <li>return</li>
  <li>Arithmetic expressions</li>
  <li>Boolean expressions (&&, ||, relational ops)</li>
  <li>Function calls</li>
  <li>Argument count validation</li>
</ul>

<h3>Operator Precedence</h3>

Correctly layered parsing:
```
OR
  → AND
    → Relational
      → Add/Sub
        → Mul/Div
          → Unary
            → Primary
```
</div>

<div>
<h2>Abstract Syntax Tree (AST)</h2>

<p>Each node stores:

Node type

Child pointers (```left```, ```right```, ```next```)

Associated symbol (if applicable)

Generated IR code head/tail pointers

The AST drives both semantic validation and code generation.
</p>
</div>
<div>
<h2>Scoped Symbol Tables</h2>

Features:
<ul>
  <li>Global scope</li>
  <li>Function scope</li>
  <li>Block-level scoping</li>
  <li>Duplicate declaration detection</li>
  <li>Undeclared variable detection</li>
  <li>Function existence checks</li>
  <li>Parameter count verification</li>
</ul>
Scope stack is managed via:

```
pushScope()
popScope()
```
</div>

<div>
<h2>Intermediate Representation (IR)</h2>

The compiler lowers AST into Three-Address Code (Quad format).

Each instruction contains:
<ul>
  <li>Operation</li>
  <li>src1</li>
  <li>src2</li>
  <li>destination</li>
  <li>optional label</li>
</ul>

Example IR (conceptual)

For:
```
x = a + b;
```

Generated IR:

```
t1 = a
t2 = b
t3 = t1 + t2
x  = t3
```
</div>

<div>
<h2>Control Flow Generation</h2>
Short-Circuit Boolean Logic

Logical ```AND``` and ```OR``` use label-based branching:

<ul>
  
```AND``` evaluates right only if left is true

```OR``` evaluates right only if left is false
</ul>

Implemented using dynamically generated labels:
```
__L0
__L1
__L2
```
</div>
<div>
<d2>While Loop Lowering</d2>

Structure:
```
goto eval
top:
    body
eval:
    if (cond) goto top
after:
```

</div>
<div>
<h2>Stack Frame Layout</h2>

Offsets are computed per function:


<ul>
  <li>Parameters: positive offsets</li>
  <li>Locals: negative offsets</li>
  <li>Temporaries: inserted dynamically</li>
</ul>
Example layout:

```
| param n   |  +offset
| param 1   |
| return addr |
| local 1   |  -offset
| local 2   |
| temp vars |
```
Offset calculation handled by:

```
get_offsets()
```
</div>
<div>
<h2>MIPS Code Generation</h2>

The backend:

<ul>
  <li>Emits ENTER / LEAVE</li>
  <li>Generates CALL / RET</li>
  <li>Manages temporaries</li>
  <li>Emits branch instructions</li>
  <li>Generates label-based control flow</li>
  <li>Produces runnable assembly</li>
</ul>

Entry stub:

```
.text
.globl main
main:
    jal _main
    li $v0, 10
    syscall
```
</div>
<div>
<h2>Example Program</h2>
  
```
  int main() {
    int x;
    x = 5;

    while (x > 0) {
        println(x);
        x = x - 1;
    }

    return 0;
}
```
</div>
<div>
<h2>Build</h2>
  
```
gcc *.c -o minic
./minic input.c > output.asm
```

Run with:

<ul>
  <li>MARS</li>
  <li>SPIM</li>
</ul>

</div>


<div>
<h1>Key Technical Highlights</h1>

<ul>
  <li>Manual AST memory management</li>
  <li>Linked-list Quad IR</li>
  <li>Short-circuit boolean lowering</li>
  <li>Dynamic temporary variable allocation</li>
  <li>Explicit stack frame construction</li>
  <li>Label-based branching</li>
  <li>Modular codegen per AST node type</li>
</ul>

</div>
