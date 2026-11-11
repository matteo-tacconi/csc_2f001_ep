# Abstract Syntax Tree

The objective of this exercise is to familiarize you with inheritance.

In this lab, we will design an interpreter for a lisp language. Lisp was created in the fifties and is the second oldest language of the world. Lisp is based on lists (lisp is an abbreviation for list processing). That means that, in lisp, a complex expression is represented by a list, where the first element of the list is an operation. When lisp evaluates a list, we say that it `applies` the operation to the list.

In lisp, an expression is written in a fully parenthesized prefix notation. For example, to represent the addition between the integers 1 and 2, we write `(+ 1 2)`. Evaluating this expression means that we apply the operation `+` on the list `(+ 1 2)`. An expression can be more complex. For example, `(* 4 (+ 1 2))` represents the multiplication between the evaluation of `4` and the evaluation of `(+ 1 2)`.

In our lab, we will represent an expression with a tree called an abstract syntax tree (AST). For example, we will represent the expression `(* 4 (+ 1 2))` with this abstract tree:

```text
   ( )
 /  |  \
*   4   ( )
       / | \
      +  1  2
```

In the figure, `( )` represents a list, and its leaves give its elements. The list at the top contains three elements: the keyword `*`, the integer `4` and a sub-list. The sub-list contains also three elements: the keyword `+` and the integers `1` and `2`.

In order to build this tree, we consider four types of nodes:

* `node_t` is a generic node. All the other types of nodes inherit from `node_t`,
* `keyword_t` is a node that represents an operation,
* `integer_t` is a node that represents an integer,
* `list_t` is a node that represents a list.

A generic `node_t` has three main methods:

* `node_t* eval()` evaluates a node and returns the result of the evaluation,
* `node_t* apply(list_t* list)` applies an operation to the list,
* `std::string to_string()` converts a node into a string in order to print it.

In order to evaluate the abstract syntax tree `(* 4 (+ 1 2))`, the interpreter call the `eval()` method of `list_t`:

* The `eval()` method of `list_t` starts by evaluating the first element of the list `(* 4 (+ 1 2))` with `eval()`. Evaluating a keyword returns the keyword, which means that evaluating the keyword `*` returns the keyword `*`. Evaluating the first element looks useless because, for the moment, we only consider a list that starts with a keyword. However, in the exercise, you will see that we can replace the first element itself by a more complex expression.
* The `eval()` method of `list_t` then calls the `apply()` method of `*` in order to apply `*` to `(* 4 (+ 1 2))`.
* The apply method of `*`, starts by evaluating the operands:
  * The evaluation of an integer simply returns the integer, which means that the evaluation of `4` returns `4`.
  * The evaluation of the list `(+ 1 2)` restarts the same process:
    * The `eval()` method of the list evaluates the first element of the list, which returns the keyword `+`,
    * The `eval()` method of the list then applies `+` to `(+ 1 2)` by calling the `apply()` method of `+`.
    * The `apply()` method of `+` applied to `(+ 1 2)` evaluates the operands, which returns the integers `1` and `2`. Then, the `apply()` method computes the sum of the intermediate results and returns the integer `3`.
* Finally, the apply method of `*` multiplies the result of the evaluation of the operands: it computes `4 * 3 = 12` and returns that integer.

> **Warning:**  
> In this lab, freeing memory is difficult. For this reason, in this exercise, you don't have to implement any destructors, and you don't have to call `delete`. The memory that you will allocate is wasted during a run, but don't worry, your operating system frees it when your process ends.
> 
> Actually, managing memory in our small interpreter is the very reason why garbage collectors were proposed. Technically, the first garbage collector was proposed to automatically manage the memory of an interpreter for the lisp language. If you want to know more about this first garbage collector, you can read this paper: McCarthy, John (1960). "Recursive functions of symbolic expressions and their computation by machine, Part I". Communications of the ACM. 3 (4): 184–195.

---

## Mandatory part

## Nodes and integers

We start our journey by implementing the `node_t` and the `integer_t` structures.

### Question 1

Implement the `node_t` structure. A `node_t` does not have any field. It also does not have constructors. It only provides the following methods:

* `virtual node_t* eval()`: by default, in `node_t`, `eval()` simply returns `this`, which is a correct behavior if the node is a keyword or a interger.
* `virtual node_t* apply(list_t* list)`: by default, in `node_t`, `apply` reports an error and quit the process (see below), which is a correct behavior if the node is an integer or a list. Note that you have to pre-declare the structure `list_t` before the definition of `node_t` in order to use the `list_t` type when you define the `apply` method in `node_t`.
* `virtual std::string to_string()`: since we don't have a satisfactory way of converting a generic node into a string, this method returns the string `???`
* `virtual list_t* as_list()`: this method convert the node into a list and will be useful in the next exercises. Since a node is not necessary a list, this function reports an error and quits.

In order to report an error and quit in `apply`, copy-paste this code:

```cpp
void error(std::string str) __attribute__((noreturn));

void error(std::string str) {
  std::cout << "Error: " << str << std::endl;
  exit(1);
}
```

The first line declare that `error` is a function that does not return. Thanks to that, the compiler will consider that not returning a `node_t*` in `apply` after having called `error` is correct. In the remainder of the lab, when you have to report an error, use this `error` function.

### Question 2

We can now implement the `integer_t` structure. This structure inherits from `node_t`. An integer has a single field: an integer with the type `uintptr_t` that gives the value of the integer. The type `uintptr_t` is a type that represents a machine word, which means 64 bits on current machines.

The `integer_t` structure has also a constructor that initializes the value, and its `to_string()` method returns a string that represents the value. For that, you can use the function `std::string std::to_string(int v)`, which builds a string from an integer. Since, in `node_t`, the behavior of `eval()` and `apply()` is already correct for a `integer_t`, you don't have to override these methods.

In main, you can now allocate an integer, evaluate it and print the result. For example, the following code should produce `3 => 3`.

```cpp
node_t* ast = new integer_t { 3 };
std::cout << ast->to_string() << " => " << ast->eval()->to_string() << std::endl;
```

🎉 **Congratulations! You have just built your first abstract syntax tree! This tree is not very impressive, but all the mechanics to execute an expression are already in place.**

## List

We will now start the implementation of the `list_t` structure. This structure is fundamentally an extensible array of `node_t*`, not so far from the army of monsters.

### Question 1

Implement a `list_t` structure. At this step, the structure contains three fields:

* `node_t** elements` points to an array, in which each element is a pointer to a node,
* `size_t nb_elements` gives the number of elements in the array.
* `size_t capacity` gives the total capacity of the array.

Additionally to the fields, add an empty constructor, i.e., a constructor that does not take arguments. The constructor has to initialize `elements` to `nullptr`, and `nb_elements` and `capacity` to 0.

### Question 2

We can now add five basic functions:

* `list_t* as_list()` returns the list.
* `size_t size()` returns `nb_elements`.
* `node_t* at(size_t i)` returns the element at the index `i`. If `i` is higher than `nb_elements`, the method uses the `error` function to report an error.
* `void set_at(size_t i, node_t* value)` sets `value` at the index `i`. If `i` is higher than `nb_elements`, the method uses the `error` function to report an error.
* `void append(node_t* value)` appends a new element to the array. If `nb_elements` is equal to `capacity`, the method extends its capacity by allocating a new array and copying the old one into the new one.
* `std::string to_string()` returns a string that represents the list with a fully parenthesized notation. For example, printing the list that contains the integer 1, 2 and 3 should return the string `(1 2 3)`.

At this step, if your code is correct, you should already be able to create and print a first list in `main`, for example with the following code in main:

```cpp
list_t* ast = new list_t;

ast->append(new integer_t { 1 });
ast->append(new integer_t { 2 });
ast->append(new integer_t { 3 });

std::cout << ast->to_string() << std::endl; // should output (1 2 3)
```

## Keywords

It's now time to create keywords.

### Question 1

We will first implement the `keyword_t` structure. This structure represents a generic keyword. We will implement a concrete keyword by inheriting from `keyword_t`. Implement the `keyword_t` structure. This data structure contains:

* an identifier `std::string id`, which identifies the keyword. The identifier will be useful when we will print complex trees,
* a constructor able to initialize this identifier,
* a `to_string()` method that returns the identifier surrounded by `<` and `>`. Surrounding the keyword with `<` and `>` will become useful later when we will introduce symbols in order to avoid mixing up a keyword with a symbol.

### Question 2

Write now a `add_t` structure that inherits `keyword_t`. For the moment, `add_t` has only to define a constructor without argument. This constructor uses the parent's constructor to initialize the identifier of the keyword to the string `+`. You can now build you first abstract tree by copying this code in `main`:

```cpp
node_t* add = new add_t;
list_t* ast = new list_t;

ast->append(add);
ast->append(new list_t);
ast->at(1)->as_list()->append(add);
ast->at(1)->as_list()->append(new integer_t { 1 });
ast->at(1)->as_list()->append(new integer_t { 2 });
ast->append(new integer_t { 3 });

std::cout << ast->to_string() << " => " << ast->eval()->to_string() << std::endl;
// should output (<+> (<+> 1 2) 3)
```

🎉 **Congratulations! You can now build abstract syntax trees! You cannot yet interpret them, but at least, when you print them, the result is pretty.**

## Abstract syntax tree interpreter

We will now bring our interpreter to life. For that, we will implement the missing `eval` and `apply` methods.

### Question 1

Instead of simply printing the abstract syntax tree of `(+ (+ 1 2) 3)`, try to evaluate it with:

```cpp
std::cout << ast->to_string() << " => " << ast->eval()->to_string() << std::endl;
```

Why your program outputs `(<+> (<+> 1 2) 3) => (<+> (<+> 1 2) 3)`?

### Question 2

We now implement the `node_t* eval()` function of `list_t`. As explained in the preamble of the lab, this function has to:

* Evaluate the first element of the list,
* call the apply method on the result of this evaluation.

Why, now, your program ends with an error?

### Question 3

As a last step, you have now to implement the `apply` method of `add_t`. For that, you have to evaluate the elements of the list by starting from the second one, and to sum up the results of these evaluations. However, since the result of an evaluation is a `node_t`, you cannot use it as a `integer_t`. Directly downcasting the result into a `integer_t` is not a good design choice since the result is not necessarily a `node_t`. For this reason, we will rely on inheritance to downcast the result. In details:

* Add `virtual uintptr_t int_value()` method in `node_t` that ends with an error explaining that the node is not an integer,
* Override `uintptr_t int_value()` in `integer_t` to return the value of the integer.

Now, you can convert the result of an evaluation to an integer, for example with `at(i)->eval()->int_value()`. After having implemented the `apply` method, your program should output `(<+> (<+> 1 2) 3) => 6`.

🎉 **Congratulations! You have now a beautiful abstract syntax tree interpreter!**

## Call frames and symbols

A language able to add values is funny, but somehow limited. For this reason, we will introduce a notion of variable. For that, we need call frames and symbols.

### Question 1

As a first step, we will create a new type of node: the `symbol_t` structure. The `symbol_t` structure inherits from `node_t` and has a single field: a `std::string` called `id`. Implement `symbol_t` with:

* A constructor that initializes `id`,
* A `bool is(std::string id)` method that returns true if `this->id` is equal to `id`.
* A `std::string to_string()` that returns `id`.

The method `bool is(std::string id)` will be called on any type of nodes later. For this reason, add also a `virtual bool is(std::string id)` method in `node_t` that returns false.

### Question 2

We will now design a call frame. In order to minimize our work, we will represents a call frame as a list, where each element of the list associates a symbol to node. In other words, a call frame is a list, where each element is a list with two elements: a symbol and a node. For example, if `frame` is a call frame, `frame->at(2)->as_list()->at(0)` gives the symbol of the second variable, and `frame->at(2)->as_list()->at(1)` gives its value.

To achieve this goal, implement a new structure called `frame_t` that inherits from `list_t` without adding fields. The `frame_t` structure implements two methods:

* `void set(node_t* sym, node_t* value)` finds the symbol `sym` in the list and updates its value. If the symbol does not yet exist, the method creates it.
* `node_t* get(node_t* sym)` returns the value associated to the symbol `sym`. If the symbol does not exist, the method reports an error.

You can observe that a symbol is considered as a `node_t*` in `get` and `set`. This is on purpose to simplify the code of the next questions. In order to know if an element is a list, you can leverage the `bool is(std::string id)` and the `std::string to_string()` method, for example with:

```cpp
if(sym->is(at(i)->as_list()->at(0)->to_string())) { ... }
```

To verify that your code is correct, copy-paste this code in `main`:

```cpp
frame_t* frame = new frame_t;

frame->set(new symbol_t { "a" }, new integer_t { 666 });
frame->set(new symbol_t { "b" }, new integer_t { 21 });
frame->set(new symbol_t { "a" }, new integer_t { 42 });

std::cout << frame->to_string() << ": a=" << frame->get(new symbol_t { "a" })->to_string() << std::endl;
```

If your code is correct, your test should output `((a 42) (b 21)): a=42`.

### Question 3

As a last step, we have to be able to convert a symbol into its value when we evaluate a symbol. The problem is that an evaluate function does not have access to the frame. For this reason, you have to add a `frame_t* frame` argument to the `eval()` methods and to the `apply(list_t* list)`. Moreover, you have pass this `frame_t*` argument step by step, from the eval methods to the apply methods, and then back from the apply methods to the eval methods.

At this question, we only focus on passing this `frame_t*` argument step by step. For that, first, add a `frame_t*` parameters in all the `eval()` (i.e., transform each `node_t* eval()` into a `node_t* eval(frame_t* frame)`). Then, add a `frame_t*` parameters in all the `apply(list_t* list)` (i.e., transform each `node_t* apply(list_t* list)` into a `node_t* apply(list_t* list, frame_t* frame)`). Finally, modify each call to `eval` or `apply` in order to pass step by step this `frame_t*` argument. For that, you can rely on the compiler, which will complain if a `frame_t*` argument is missing.

### Question 4

Now, implement the `node_t* eval(frame_t* frame)` method of `symbol_t`. This function has to return the value associated to the symbol in the frame `frame`. If your code is correct, the following code should print `(<+> (<+> a 2) 3) => 42`.

```cpp
node_t* add = new add_t;
symbol_t* a = new symbol_t { "a" };
frame_t* frame = new frame_t;

frame->set(a, new integer_t { 37 });

list_t* ast = new list_t;

ast->append(add);
ast->append(new list_t);
ast->at(1)->as_list()->append(add);
ast->at(1)->as_list()->append(a);
ast->at(1)->as_list()->append(new integer_t { 2 });
ast->append(new integer_t { 3 });

std::cout << ast->to_string() << " => " << ast->eval(frame)->to_string() << std::endl;
```

## The set! keyword

For the last exercise of the mandatory part, you will implement a `set!` keyword. This keyword stores a value into a variable. For example, the expression `(set! a (+ 1 2))` stores the integer 3 into the variable `a`. In order to achieve that goal, the `set!` keyword has use `frame->set()` to set the value of the variable (and to create it if it does not yet exist).

### Question 1

Implement the `set!` keyword with a structure called `set_t`.

🎉 **Congratulations! You were able to implement a basic but working abstract syntax tree interpreter!**

At this step, you probably understand how you can easily add new keywords. For that, you have to create a new structure that inherits from `keyword_t` and to define a new `node_t* apply(frame_t* frame, list_t* list)` method. For example, if we supposed an inequality test operator implemented (i.e., a `<` keyword), implementing a loop to evaluate an expression such as `(while (< i 0) (set! i (+ i 1)))` is as simple as:

```cpp
node_t* apply(frame_t* frame, list_t* list) {
  node_t* res = 0;

  while(list->at(1)->eval(frame)->int_value() != 0)
    for(size_t i=2; i<list->size(); i++)
      res = list->at(i)->eval(frame);

  return res;
}
```

---

## Non mandatory part

For the students interested by the subject, you can continue the exercise to see how you complete the runtime for our lisp language. This part is not graded and is not mandatory. It is only here if you are curious. Implementing the whole interpreter does not require many lines of code: the code of the complete runtime is roughly 400 lines long.

## Lexer and parser

Manually building the abstract syntax trees as we have done in the first part is long and boring. For this reason, before implementing new interesting keywords, we will need a front-end for our language runtime. The front-end takes as entry a source code and uses it to build the abstract syntax tree.

Traditionally a language front-end is built in two parts: a lexer and a parser. A lexer is in charge of identifying the words of the language, which are usually called the `token`. For example, with a sentence like `(+ (+ var 12) 42)`, the lexer will generate this stream of tokens: `(`, `+`, `(`, `+`, `var`, `12`, `)`, `42` and `)`. In this stream, `(`, `+`, `var`, `)` are symbols, while `12` and `14` are integers. In a most language, the lexer directly generates keyword nodes for the keywords and symbols for the symbols, but in lisp, it is much easier to only consider symbols at this step.

The parser consumes the stream of tokens. It is in charge of building the abstract syntax tree.

### Question 1

We will start by implementing the lexer. For that, you have to define a `lexer_t` structure. This structure has two parameters:

* `std::istream* is` is an input stream. This class has one useful method in the exercise: the `int get()` method, which reads a character from the stream.
* `int next` gives the next character. Keeping this character is required because, sometimes, two tokens are not separated by a space. For example, when the lexer analyzes `12)` it finds the end of the integer `12` by reading the character `)`. At this step, the lexer returns the `integer_t` 12. Since `)` is already consumed from the input stream, we have to keep it somewhere: this is the goal of the `next` field.

Implement a first version of the `lexer_t` structure with these two fields. Add a constructor that initializes `is` from a parameter, and `next` to `' '`. Considering that the next character is `' '` is correct since the lexer ignores them. In main, create a lexer with the expression `lexer_t lexer(&std::cin)`. The `std::cin` object is the standard input stream and reads the characters from the terminal.

### Question 2

Add a method `node_t* next_token()` in `lexer_t`. The method has first to ignore the spaces. In details, while next is a space, you have to read a new character and to store it in `next`. In order to know if a character is a space, you can use `std::isspace(int c)`, which return true if the character is a space (i.e., ' ', '\n', a tabulation etc.).

Then, depending on the first character that is not a space, we have several choices:

* If `next` is equal to EOF, which represents the end of a file, it's an error: the stream is not supposed to end abruptly.
* If `next` is equal to a parenthesis, we have to allocate a symbol initialized with the character, to call `is->get()` to prepare the next character for the next token, and to return the symbol.
* Otherwise, it means that the character starts either an integer or a symbol. In order to handle both cases, you need three local variables:
  * `uintptr_t number = 0` gives the value of the number if it happens that we are reading a number,
  * `std::string str = ""` gives the value of the symbol if it happens that we are reading a string,
  * `bool is_number = true` indicates whether we are reading an integer or string. Initially, we consider that we are reading an integer, and if we read a character that is not an integer, we will switch this boolean to false.
  
  After having initialized the three variables, you need an infinite loop. This loop:
  * sets `is_number` to false if `next` is not a number, i.e., if `next` is strictly lower than the character '0' or if `next` is strictly higher than `'9'`.
  * updates `number` by multiplying it by 10, and by adding `(next - '0')` to it since `(next - '0')` gives the integer associated to the character,
  * appends `next` to `str` by using the `+=` operator,
  * reads the next character and stores it in `next`,
  * at this step, if the new next is a space or a parenthesis, it means that we have our token. We can just return it: an integer initialized with `number` if `is_number` is true, or a symbol initialized with `str` otherwise.

In main, add an infinite loop that reads tokens, and then prints them by using the `to_string()` method. Test that your code is correct with different expressions such as `(+ (+ var 12) 42)`. Your program should print the different tokens, i.e., `(`, `+`, `(`, `+`, `var`, `12`, `)`, `42` and `)`.

> **Note:**  
> To quit your program, you have to type Control + c

### Question 3

We can now implement our parser, which builds an abstract syntax tree by consuming the token generated by the lexer. For that, implement a method `node_t* parser(lexer_t* lexer)`. The parser is surprisingly simple if we implement it recursively. In details, the parser reads a token, and if the token is not the `"("` symbol, it returns it (recall that you have a `bool is(std::string id)` to know if a node is the symbol `"("`). Otherwise, i.e., if the token is an opening parenthesis, the `parser` function creates a list, and call recursively `parser` in a loop that ends when the token is the symbol `")"`. At each step of the loop, the `parser` function simply appends the node returned by the recursive call to the list.

In main, in the infinite loop, instead of using the lexer, print the abstract syntax tree returned by the parser.

### Question 4

We can now evaluate our abstract syntax trees and print the result in the infinite loop. For that, you have to create a frame before the infinite loop, and to fill it with our two keywords since the abstract syntax tree does not contains the keywords, but the symbols that represent the keyword. For example, to resolve the symbol `+` to the keyword `add_t`, you simply have to write `frame->set(new symbol_t { "+" }, new add_t)`. If your code is correct, you can already performed addition and use variables. Impressive, isn't it?

## Exception and error handling

### Question 1

Terminating our program when the user enter an invalid an expression is not user-friendly. Instead of quitting, we will rely on what is called an exception. Without giving the details, in `error`, replace all the code by the expression `throw str;`. This expression throws an exception, which means that it inspects the frames of the callers one by one, up to reaching a point where the exception is catched. In the infinite loop in `main`, surround the code by `try { old code here } catch(std::string str) { std::cout << "Error: " << str << std::endl; }`. You can test that now, when an error occurs (e.g., because you try to execute `(1 2)`), your program restarts its execution in the main loop.

### Question 2

Before going further, we need some keywords. Implement the following keywords:

* `-` such as in `(- x)` or `(- x 2)`: inverts the parameter if the list has a single parameter, computes a subtraction otherwise.
* `=`, `<` etc. such as in `(< x 10)`: compares two nodes, by supposing that they are integer,
* `if` such as in `(if (= x 10) 0)` or `(if (= x 10) 0 1)`: implements a conditional. If the conditional is false and if the list has only two elements, you can return the integer 0.
* `begin` such as `(begin (set! x 1) (set! y 2))`: groups together a set of expression. In details, it executes the expression one after the other and returns the last.

At this step, you should be able to execute a complex code such as:

```lisp
(while (< x 10)
  (if (= x 2)
      (begin
        (set! y 42)
        (set! z 100))))
```

## Lambda expression

We will now implement the functions and the function calls. For that we will use what we call a lambda expression. A lambda expression is a list that starts by the keyword `lambda`. Additionally to the keyword, a lambda expression contains two other elements: a list of parameters and a body. For example `(lambda(x) (+ x 1))` represents a function with a single parameter named `x`, and that returns the value `(+ x 1)`.

When we evaluate a lambda expression, we cannot yet call the function: technically, when we evaluate `(lambda(x) (+ x 1))`, we don't know yet what `x` will be. For this reason, the `node_t* eval(frame_t* frame)` function of a lambda expression returns the lambda expression. Like that, we can, for example, save a lambda expression in a symbol, like in `(set! f (lambda(x) (+ x 1)))`. If you evaluate `f` in this case, your program will thus also print the lambda expression.

In order to call a lambda expression, we have to use it, for example with `(f 2)`, which calls the lambda expression with the parameter 2. Since the evaluation of `f` returns the lambda expression, the interpreter will call the `apply` method of `(lambda(x) (+ x 1))` with the initial list as parameter, i.e., `(f 2)`. Since `(lambda(x) (+ x 1))` is a list, it is the `apply` method of `list_t` that is called. A `list_t` is thus the interpreter of a lambda expression. The `node_t* apply(frame_t* frame, list_t* list)` methods of `list_t` has to:

* Create a new frame `new_frame` to store the local variables of the function that will be called,
* Create the parameters in `new_frame`. For that, `apply` has to iterate over the elements of the second element of `this` (`this->at(1)`, `this->at(1)` etc.), and for each symbol, it has to set the symbol to the evaluation of the argument (i.e., `list->at(1)->eval(frame)`, `list->at(2)->eval(frame)` etc.).
* Start the execution of the body, technically by evaluating it with the new frame (`this->at(2)->eval(new_frame)`).

### Question 1

With the algorithm described above, `new_frame` only contains the local variable, but not the keywords, which are defined in the root frame created in the `main` function. In order to retrieve them, we will link the frame of a callee to the frame of its caller. Technically, it means that you have to add a `frame_t* parent` field in `frame_t`, and to initialize it through a constructor. For the root frame, i.e., the initial frame created in main, the parent has to be set to `nullptr`.

Additionally to adding a parent field, modify the method `get` to try to find a symbol in the parent's frame if the variable is not defined in the frame.

### Question 2

Add the lambda keyword. The `apply` method of the lambda keyword simply returns the list parameter without evaluating it. Try to execute:

```lisp
(set! f (lambda(x) (+ x 1)))
f
```

If your program is correct, the interpreter should just print the lambda expression.

### Question 3

Add the `eval` method in `list_t`. The method is in charge of invoking the lambda expression as described above.

🎉 **Congratulations! You made it, you wrote your first language runtime for a lisp language!**

## Making all that magic

Our language runtime cannot communicate with the user yet. For that, you have to access C functions such as `printf`, which rely on the operating system to communicate with the user. These functions are said to be foreign functions, since they are not defined in the target language (i.e., lisp in our case). In order to invoke a foreign function, we need a wrapper. For that, we could define our wrappers as keywords, which would solve the problem. However, writing these keywords will become quite repetitive. For this reason, we will use a more generic solution, which consists in generating on demand the wrappers to invoke the foreign functions.

To generate on demand these wrappers, we will rely on a function provides by the `C` library called `dlsym(void* handle, const char* name)`. `dlsym` looks up, in the binary file or shared library represented by `handle`, the value associated to the symbol named `name`. This is similar to what you have done with the `get` method of `frame_t`, which looks up a value associated to a symbol name. In our case, `dlsym` becomes interesting with the predefined `RTLD_DEFAULT` library, which represents our own binary. With that, we can extract the address of a symbol inside our own binary. For example, to extract the address of `printf`, you can write `dlsym(RTLD_DEFAULT, "printf")`.

> **Note:**
> If you want to see all the symbols of your binary, you can use the command `nm`. The C symbols are encoded as they written, but the C++ symbols are encoded in a strange way with their types. To see the corresponding symbols, you can chain `nm` with `c++filt`. For example, the following command prints all the C++ symbols with their associated encoded names:
>
> ```bash
> nm -j ./ast | while read s; do printf "%-60s => %s\n" "$(c++filt $s)" "$s"; done
> ```
>
> If you're looking for a specific symbol, you can chain the result of the previous command with `grep`. For example, to see all symbols of the `list_t` structure, you can use
>
> ```bash
> nm -j ./ast | while read s; do printf "%-60s => %s\n" "$(c++filt $s)" "$s"; done | grep 'list_t::'
> ```

### Question 1

In order to extract a symbol of our library, we need strings. Implement a `string_t` structure that inherits `node_t`. A `string_t` has a single field with the type `std::string` that represents the string, and its `std::string to_string()` method simply returns it.

Modify the lexer to be able to parse a string. For that, if the token starts by `"`, you have to accumulate the characters in the `str` local variable, and to return the string when you find the ending `"` (don't forget to call `is->get()` before returning to prepare the next token). While you process an integer or a symbol, you also have to stop when you find a `"`, since this symbol indicates that you are starting a string. Check that you are now able to parse string by typing `"hello"` in the interpreter. If your code is correct, your interpreter should output `hello`.

### Question 2

We can now create our foreign function interface (FFI). For that, we use a trick: we consider that if a list starts with an integer, then the integer is the address of a function. For this reason, the `void apply(frame_t* frame, list_t* list)` will act as our foreign function interface. Since writing the code to indirectly call a function through a pointer in a generic way is difficult, we provide the code of this `apply` method:

```cpp
node_t* integer_t::apply(frame_t* frame, list_t* list) {
  uintptr_t (*f)(uintptr_t, uintptr_t, uintptr_t, uintptr_t) = (uintptr_t (*)(uintptr_t, uintptr_t, uintptr_t, uintptr_t))int_value();
  uintptr_t (*g)(uintptr_t, ...) = (uintptr_t (*)(uintptr_t, ...))int_value();
  std::string strs[4];
  uintptr_t args[] = { 0, 0, 0, 0 };
  size_t n = list->size() < 5 ? list->size() : 5;

  for(size_t i=1; i<n; i++) {
    node_t* arg = list->at(i)->eval(frame);
    std::cout << "str: '" << arg->to_string() << "'" << std::endl;
    args[i-1] = arg->is_integer() ? arg->int_value() : (uintptr_t)(strs[i-1] = arg->to_string()).c_str();
  }

  if((uintptr_t)f == (uintptr_t)printf || (uintptr_t)f == (uintptr_t)vprintf || (uintptr_t)f == (uintptr_t)snprintf)
    return new integer_t { g(args[0], args[1], args[2], args[3]) };
  else
    return new integer_t { f(args[0], args[1], args[2], args[3]) };
}
```

To understand the code, `f` is a pointer to a function with 4 arguments (the maximum number of arguments that we handle), while `g` is a pointer to a function with a variadic number of arguments. `f` and `g` are initialized with the `int_value()` of the integer, which is supposed to be a function pointer. The loop is in charge of extracting the arguments and of preparing them for the foreign call. To differentiate an integer and a string, the code uses `is_integer()` that you have to implement. The last line uses `g` for the common variadic calls (e.g., the `printf` functions family), and `f` otherwise.

> **Note:**
> To make the code run, you have to implement an `is_integer()` that returns false for all the node types but `integer_t`.

### Question 3

Now that we have a foreign function interface, we can expose the `dlsym` function and the `RTLD_DEFAULT` handle in the language with:

```cpp
frame->set(new symbol_t { "dlsym" }, new integer_t { (uintptr_t)dlsym });
frame->set(new symbol_t { "dlself" }, new integer_t { (uintptr_t)RTLD_DEFAULT });
```

If everything works well, you can use `dlsym` to create on demand new foreign functions interface. If your code is correct, you should be able to execute this program:

```lisp
(begin
  (set! printf (dlsym dlself "printf"))
  (set! exit (dlsym dlself "exit"))
  (printf "\n=== welcome ===\n")
  (set! f (lambda(x)
            (begin
              (set! i 0)
              (while (< i x)
                     (begin
                       (printf "i: %d\n" i)
                       (set! i (+ i 1)))))))
  (set! g (lambda(x) (f (+ x 2))))
  (g 3)
  (exit 42))
```

---

🎉 **Congratulation, you have a basic complete language runtime. The runtime is not perfect and can be extended, but adding new features now is only a matter of engineering: all the basics elements of any language runtime are present.**



