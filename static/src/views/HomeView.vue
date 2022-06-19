<script setup>
 import { ref } from 'vue';

 const transpilationInput = ref("1 + 2.");
 const transpilationOutput = ref("");

 function transpile()
 {
     fetch(`http://cat-lang.herokuapp.com/api/v1/transpilation`, {
         method: "POST",
         headers: {"Content-Type": "application/json"},
         body: JSON.stringify({data: transpilationInput})
     }).then(json => JSON.parse(json))
       .then(result => transpilationOuput.value = result)
       .catch(error => console.log(error));
 }
</script>

<template>
    <main>
        <div id="toolbar" class="main-item">
            <button id="run-btn" @click="transpile">
                RUN
                <font-awesome-icon icon="fa-solid fa-play" />
            </button>
        </div>

        <div id="transpilation-input" class="main-item">
            <textarea autofocus v-model="transpilationInput"></textarea>
        </div>

        <div id="transpilation-output" v-if="transpilationOutput != ''" class="main-item">
            <span id="transpilation-output-header">Transpilation</span>
            <pre id="transpilation-output-output"><code>{{ transpilationOutput }}</code></pre>
        </div>

    </main>
</template>

<style scoped>
 main {
     display: flex;
     flex-direction: column;
     padding: 0 1rem;
 }

 .main-item {
     padding: 0.5rem 0;
 }

 toolbar {
     display: flex;
 }

 #toolbar button {
     padding: 0.8rem;
     background-color:white;
     font-weight: bold;
     color: black;
     margin-top: 0.5rem;
     border-radius: 5px;
     transition: all 0.2s;
     cursor: pointer;
 }

 #toolbar button:hover {
     background-color: #eee;
 }


 #transpilation-input textarea {
     padding: 1rem;
     border-radius: 5px;
     resize: vertical;
     height: 200px;
     width: 100%;
 }

 #transpilation-output {
     width: 100%;
     display: flex;
     flex-direction: column;
     align-items: center;
     border: 1px solid #bbb;
     padding: 0.5rem;
 }

 #transpilation-output-header {
     border-bottom: 1px solid #bbb;
     text-align: center;
     padding-bottom: 0.5rem;
     margin-bottom: 0.5rem;
     width: 100%;
 }

 #transpilation-output-output {
     width: 100%;
 }
</style>
