<script setup>
 import { ref } from 'vue';
 import PopupNotification from '@/components/PopupNotification.vue'

 const showClipboardCopiedNotification = ref(false)
 const loading = ref(false)
 const transpilationInput = ref("1 + 2.");
 const transpilationOutput = ref("");
 const executionOutput = ref("");

 const htmlEntities = (s) => s.replaceAll("<", "&lt").replaceAll(">", "&gt");

 function preprocessOutput(s)
 {
     return htmlEntities(s)
         .replace(/\[31m/g, "<span style='color: red; font-weight: bold;'>")
         .replace(/\[34m/g, "<span style='color: cyan; font-weight: bold;'>")
         .replace(/\[m/g, "</span>")
         .replace(/[^-a-zA-Z0-9\<\>\n\[\]=:';\/&.$,?^()| ]/g, '');
 }

 function copyToClipboard(nodeCoords, extractorFn)
 {
     const node = document.querySelector(nodeCoords)
     navigator.clipboard.writeText(extractorFn(node))
     showClipboardCopiedNotification.value = true
     setTimeout(() => showClipboardCopiedNotification.value = false, 1000)
 }

 const copyMIPS = () => copyToClipboard("#transpilation-output-output code", node => node.innerText);
 const copySRC = () => copyToClipboard("#transpilation-input textarea", node => node.value)

 const transpile = async() => {
     loading.value = true;

     const resp = await fetch(`https://cat-lang.herokuapp.com/api/v1/transpilation_and_execution`, {
         method: "POST",
         headers: {"Content-Type": "application/json" },
         body: JSON.stringify({data: transpilationInput.value})
     });

     const data = await resp.json();

     transpilationOutput.value = preprocessOutput(data["data"]["transpilation_result"]);

     if (resp.ok) {
         executionOutput.value = data["data"]["execution_result"];
     }

     loading.value = false;
 }
</script>

<template>
    <PopupNotification content="Copied!" :showing="showClipboardCopiedNotification" />
    <main>
        <div id="toolbar" class="main-item">
            <button id="run-btn" @click="transpile" title="Ctrl + Enter">
                RUN <font-awesome-icon icon="fa-solid fa-play" />
            </button>
            <button id="clip-src-btn" @click="copySRC">
                Copy Source <font-awesome-icon icon="fa-solid fa-clipboard" />
            </button>
            <button id="clip-mips-btn" @click="copyMIPS">
                Copy MIPS <font-awesome-icon icon="fa-solid fa-clipboard" />
            </button>
            <img v-show="loading" src="https://bison.usgs.gov/images/spinner2.gif" width="40" height="40" />
        </div>

        <div id="transpilation-input" class="main-item" >
            <textarea v-model="transpilationInput" @keyup.ctrl.enter="transpile" spellcheck="false" />
        </div>

        <div id="bottom-pane" class="main-item">
            <div id="transpilation-output" >
                <span id="transpilation-output-header">Transpilation</span>
                <pre id="transpilation-output-output"><code v-html="transpilationOutput"></code></pre>
            </div>
            <div id="execution-output">
                <span id="execution-output-header">Execution</span>
                <pre id="execution-output-output"><code v-html="executionOutput"></code></pre>
            </div>
        </div>

    </main>
</template>

<style lang="scss" scoped>
 main {
     display: flex;
     flex-direction: column;
     padding: 0 1rem;

     & > div {
         padding: 0.5rem 0;
     }
 }

 #toolbar {
     display: flex;
     align-items: center;

     button {
         padding: 0.8rem;
         margin: 0 0.2rem;
         color: white;
         background-color: #b7372b;
         font-weight: bold;
         margin-top: 0.5rem;
         border: none;
         border-radius: 5px;
         transition: all 0.2s;
         cursor: pointer;

         &:hover {
             background-color: #95372b;
         }
     }
 }

 #transpilation-input {
     textarea {
         outline: none;
         padding: 1rem;
         border: 1px solid #bbb;
         border-radius: 5px;
         resize: none;
         height: 200px;
         width: 100%;
     }
 }

 #bottom-pane {
     display: grid;
     grid-template-columns: repeat(2, 1fr);
     grid-gap: 0.5rem;

     #transpilation-output, #execution-output {
         width: 100%;
         display: flex;
         flex-direction: column;
         align-items: center;
         border: 1px solid #bbb;
         background-color: white;
         padding: 0.5rem;

         span {
             border-bottom: 1px solid #bbb;
             text-align: center;
             padding-bottom: 0.5rem;
             margin-bottom: 0.5rem;
             width: 100%;
         }

         pre {
             width: 100%;
             height: 200px;
             overflow-y:auto;
         }
     }
 }
</style>
