<script setup>
import { computed } from 'vue';

 const props = defineProps({
     user: String,
     repo: String
 });

const githubUrl = computed(() => `https://github.com/${props.user}/${props.repo}`)
</script>

<template>
    <div class="github-ribbon"
         style="position: absolute; width:
                150px; height: 150px; overflow: hidden; z-index: 99999; right:
                0px; top: 0px;"
    >
        <a target="_blank"
           style="display: inline-block;
                   width: 200px; overflow: hidden; padding: 6px 0px; text-align:
                   center; transform: rotate(45deg); text-decoration: none; color:
                   rgb(255, 255, 255); position: inherit; font: 700 13px
                   &quot;Helvetica Neue&quot;, Helvetica, Arial, sans-serif;
                   box-shadow: rgba(0, 0, 0, 0.5) 0px 2px 3px 0px;
                   background-color: rgb(170, 0, 0); right: -40px; top: 45px;
                   background-image: linear-gradient(rgba(0, 0, 0, 0), rgba(0, 0,
                   0, 0.15));"
           :href="githubUrl"
        >
            Fork me on GitHub
        </a>
    </div>
</template>

<style scoped>
 .github-ribbon {
     position: absolute;
     width: 150px;
     height: 150px;
     overflow: hidden;
     z-index: 99999;
     right: 0px;
     top: 0px;
 }

 .github-ribbon a {
     display: inline-block;
     width: 200px;
     overflow: hidden;
     padding: 6px 0px;
     text-align: center;
     transform: rotate(45deg);
     text-decoration: none;
     color: rgb(255, 255, 255);
     position: inherit;
     font: 700 13px "Helvetica Neue", Helvetica, Arial, sans-serif;
     box-shadow: rgba(0, 0, 0, 0.5) 0px 2px 3px 0px;
     background-color: rgb(170, 0, 0);
     right: -40px;
     top: 45px;
     background-image: linear-gradient(rgba(0, 0, 0, 0), rgba(0, 0, 0, 0.15));
 }
</style>
