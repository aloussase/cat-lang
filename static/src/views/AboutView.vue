<script setup>
 import TableOfContents from '@/components/TableOfContents.vue';
 import CatCodeBlock from '@/components/CatCodeBlock.vue';
</script>

<template>
    <TableOfContents
        :links="[
            ['Arithmetic Operators', '#arithmetic-operators'],
            ['Variables', '#variables'],
        ]" />
    <main>
        <section id="about">
            <p>
                Cat is simple programming language that transpiles to MIPS assembly.
            </p>
            <p>
                Try entering some commands in the editor to see what happens,
                Cat took inspiration on Rust and Jakt for good error reporting, so you
                can learn the language by making mistakes!
            </p>
        </section>

        <section id="arithmetic-operators">
            <h2>Arithmetic Operators</h2>
            <p>
                Cat has three arithmetic operators: <code>+</code>, <code>-</code> and <code>*</code>.
                All three are left associative and multiplication has higher precedence than addition
                and subtraction. The latter have the same precedence.
            </p>
            <CatCodeBlock>
                1 + 2 * 3.
            </CatCodeBlock>
            <p>The result of the above expression will be 7, while the result of</p>
            <CatCodeBlock>
                (1 + 2) * (4 - 2) * 2.
            </CatCodeBlock>
            <p>would be 12.</p>
            <p>
                Addition translates to an <code>add</code> or <code>addi</code> instruction in MIPS
                depending on whether an immediate value is available (which they are in the examples above).
                Multiplication uses <code>mult</code> followed by <code>mflo</code> to move the result into
                a usable register (note that this means that the result must fit in 32 bits).
            </p>
        </section>

        <section id="variables">
            <h2>Variables</h2>

            <p>
                Cat variables must be declared with the keyword <code>let</code> before being used, using
                an undeclared symbol is an error. The assigment operator is <code>:=</code>.
            </p>
            <CatCodeBlock>
                let x := 1 + 2.
            </CatCodeBlock>
            <p>Once declared and initialized, a variable can be reassigned:</p>
            <CatCodeBlock>
                x := 42.
            </CatCodeBlock>
            <p>Note that assignment is an expression, so the following code is valid:</p>
            <CatCodeBlock>
                let y := x := 3.
            </CatCodeBlock>
            <p>
                After executing that line, <code>y</code> and <code>x</code> will both have the value
                of <code>3</code>.
            </p>
            <p>
                Variables are implemented by storing their values in the stack and keeping track of offsets
                to be able to retrieve them. The code <code>let x := 12.</code> would more or less
                produce the following MIPS code:
            </p>
            <pre>
add $sp, $sp, 4
li  $t0, 12
sw  $t0, 0($sp)
            </pre>
        </section>
    </main>
</template>

<style lang="scss" scoped>
 main {
     width: 60%;
     margin: auto;

     section{
         padding: 1rem 0;

         p {
             margin: 0.7rem 0;
         }
     }
 }
</style>
