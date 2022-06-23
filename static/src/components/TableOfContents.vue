<script setup>

 const props = defineProps({
     links: Object,
 });

</script>

<template>
    <ul>
        <li v-for="[link, to] in links">
            <a :href="to">{{ link }}</a>
        </li>
    </ul>
</template>

<style lang="scss" scope>
 ul {
     position: fixed;
     top: 0;
     left: 0;
     bottom: 0;
     background-color: white;
     padding: 1rem 2rem 0 0.5rem;
     list-style: none;
     overflow-y: auto;

     li {
         margin-bottom: 0.5rem;

         a {
             color: black;
             text-decoration: none;

             &:hover { color: blue; }
         }
     }
 }
</style>
