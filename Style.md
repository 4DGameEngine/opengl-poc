# C++ Coding Style Guide
Please follow these whenever possible/sane from now on.

## The Golden Rule
Be Sensible. If there is something really wrong with following the guidelines, and
there is a reasonable alternative, then do it.

## GLSL
* GLSL files should end in '.glsl'
* GLSL files should follow roughly the same formatting as C++, but not really strict on this.

## Indentation, Line Breaks
* Lines should be STRICTLY 80 CHARACTERS OR LESS. (Whenever sane.)
* Indents should be 4 spaces wide.
* Indents should be expanded to spaces, except for Makefiles.
* If you are breaking a line into multiple lines, align them appropriately:
```
// Examples:
// Functions and function calls:
glm::mat4 lookAt4D(const glm::vec4 &from, const glm::vec4 &to,
                   const glm::vec4 &up, const glm::vec4 &over);

// Assignment
int randomness = array[hello] * array[hello] + array[hello+1] * array[hello+1]
                 + array[wow] * array[wow] + array[wow+1] * array[wow+1]
                 + something;
```
* Insert empty lines into your code to delimit logical blocks.
* Also see indentation for classes in a later section.

## Naming Conventions
* As far as possible, use `camelCase` for names for names of functions and
variables.
* Use `CamelCase` beginning with a capital for class names, to differentiate
from library classes.
* Use `ALL_CAPS_WITH_UNDERSCORES` for global variables and macros.
  * Header guards should be of the form `_NAME_H_`.
* Use descriptive names for variables.
  * Descriptive doesn't necessarily mean long, especially for temporary vars.
  * Global variables (ideally, not many or none) and macros should be very 
    clear and descriptive (and perhaps a little longer).

## Arithmetic
* Put spacing between all arithmetic operations and assignments, except for:
  * Division
  * Unary operators
  * When the calculation is done inline for an array index e.g. `array[i*3+2]`

## Brackets
* No space before the () when calling or declaring a function.
* In other locations where brackets would appear, put a space before.
  * For example, while/for loops, if statements, etc.
* If you think precedence is unclear, use brackets.
* Don't have too many layers of brackets. That usually indicates that you are
doing too many things at once.

## Looping and Functions
* If you have more than three levels of nested loops, rethink how you are
structing your code.
* Abstract away repeated code into functions (which may eventually become
objects.
  * Try not to do so prematurely.
  * You may want to consider breaking up a monolithic function into smaller
    function calls as well, even if the code is not repeated. But don't
    overdo it.

## Templates
* Either put the template parameters on the line before (if they are too long)
or on the same line as the function/class declaration, use your discretion.

## Pointers and References
* Attach the `*` or the `&` to the variable, not the type. No spaces before
the variable name.

## Comments
* Put a space after starting a comment with `//` e.g. `// Hi`, not `//Hi`.
* Use multiline comments if you want to.
* Don't comment too much.
  * In particular, I feel that the tutorials have too many comments, don't
    just copy all of them.
* You usually don't want to comment on how a piece of code works, you want to
comment on what it does. If you have to explain how it works, your code is
probably not written transparently enough.

## Braces
* You may omit braces from single line conditionals/loops, but try not to do
so if there is too much nesting or extra lines are likely to be added at that 
location.
* Braces should follow the K & R style, more or less:

```
// Functions
void myFunction(int a, int b)
{
    // Note that function and class braces are different.
    // stuff
}

// Classes
// Note the spacing and indentation.
class a : public otherClass {
    public:
        int thing;
        int otherthing; 
    private:
        int hi;
}

// Conditionals
if (a == 0) {
    // stuff
} else {
    // stuff
}

// Loops
while (a > 0) {
    // stuff
}

for (int i = 0; i < 2; i++) {
    // stuff
}

// Note the spacing of the ':' as well.
for (auto &i : myVec) {
    // stuff
}

// Huge arrays
GLfloat vertices[] = {
    ...
    // Note that we end with a comma
    -0.8f,   0.8f,  -0.8f,  -0.8f,
};
```

## Committing and Pushing
* Before pushing code to the repository, make it clean and readable.
* Having some debugging stuff in is fine, but in particular, don't:
  * Clutter up the code with huge sections (or even multiple small sections)
    of tests, especially if they have been commented out.
  * Don't leave hanging variables that are unused, or anything like that. 
