<script setup>
 import { ref, onMounted } from 'vue';

 const code = ref(null);

 function syntaxHighlight(s)
 {
     return s.replace(/let/g, match => `<span class="cat-keyword">${match}</span>`)
             .replace(/[1-9]/g, match => `<span class="cat-number">${match}</span>`);
 }

 onMounted(() => code.value.innerHTML = syntaxHighlight(code.value.innerHTML));
</script>

<template>
    <pre ref="code" class="cat-code"><slot></slot></pre>
</template>

<style lang="scss">
 @import url('https://fonts.googleapis.com/css2?family=JetBrains+Mono&display=swap');

 .cat-code {
     font-family: 'JetBrains Mono', monospace;
     background-color: #f6f7f6;
     padding: 0.5rem;
 }

 .cat-keyword  { color: #9d00ec; }
 .cat-number   { color: #b21e00; }
</style>
