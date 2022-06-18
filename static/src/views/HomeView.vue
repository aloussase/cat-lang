<script setup>
 import { ref } from 'vue';
 import ForkMe from '@/components/ForkMe.vue';

 const transpilationInput = ref("1 + 2.");
 const transpilationOutput = ref("");

 function transpile()
 {
     fetch(`http://localhost/api/v1/transpilation`, {
         method: "POST",
         headers: {
             "Content-Type": "application/json"
         },
         body: JSON.stringify({data: transpilationInput})
     })
     .then(json => JSON.parse(json))
     .then(result => transpilationOuput.value = result)
     .catch(error => console.log(error));
 }
</script>

<template>
    <main>
        <ForkMe />
        <h1>Try Cat!</h1>
        <section id="transpilation-input">
            <form id="transpilation-form">
                <textarea autofocus v-model="transpilationInput"></textarea>
                <input type="submit" value="Run"  @click.prevent="transpile" />
            </form>
        </section>
        <section id="transpilation-output">
            {{ transpilationOutput }}
        </section>
    </main>
    <footer>
        <small>&#169; Alexander Goussas 2022</small>
    </footer>
</template>

<style scoped>
 @import url('https://fonts.googleapis.com/css2?family=Raleway&family=VT323&display=swap');

 * {
     margin: 0;
     padding: 0;
     box-sizing: border-box;
 }

 body {
     font-family: 'Raleway', sans-serif;
 }

 main {
     display: flex;
     flex-direction: column;
     align-items: center;
     justify-content: center;
     min-height: 100vh;
     width: 80%;
     margin: auto;
 }

 h1 {
     margin: 1rem 0;
 }

 #transpilation-form {
     display: flex;
     flex-direction: column;
     width: 100%;
 }

 #transpilation-form input[type=submit] {
     padding: 0.5rem;
     background-color:white;
     color: black;
     margin-top: 0.5rem;
     border-radius: 5px;
     transition: all 0.2s;
     cursor: pointer;
 }

 #transpilation-form input[type=submit]:hover {
     background-color: #eee;
 }


 #transpilation-form textarea {
     padding: 1rem;
     background-color: #000;
     color: #00ff00;
     font-size: 1.2rem;
     border-radius: 5px;
     resize: none;
     font-family: 'VT323', monospace;
     height: 300px;
     width: 600px;
 }

 #transpilation-result {
     margin-top: 1rem;
     padding: 0.5rem;
     background: gray;
     width: 100%;
 }

 #transpilation-error {
     color: red;
 }

 /* Footer */
 footer {
     display: flex;
     justify-content: center;
     padding: 0.8rem;
 }

 /* media queries */
@media screen and (max-width: 480px) {
    main { width: 90%; }
    h1 { font-size: 1.5rem; }
    #transpilation-form textarea { height: 300px; }
}

</style>
