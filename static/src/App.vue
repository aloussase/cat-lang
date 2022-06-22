<script setup>
 import { RouterLink, RouterView } from 'vue-router'
 import ForkMe from '@/components/ForkMe.vue';
</script>

<template>
    <header>
        <div class="wrapper">
            <nav>
                <RouterLink to="/">Home</RouterLink>
                <RouterLink to="/about">About</RouterLink>
            </nav>
        </div>
    </header>
    <ForkMe user="aloussase" repo="cat-lang" />
    <RouterView />
</template>

<style>
 @import url('https://fonts.googleapis.com/css2?family=Raleway&family=VT323&display=swap');

 * {
     margin: 0;
     padding: 0;
     box-sizing: border-box;
 }

 body {
     font-family: 'Raleway', sans-serif;
     background-color:  #fdf5e7;
 }

 nav {
     padding: 1rem;
     width: 100%;
     display: flex;
     justify-content: center;
 }

 nav a {
     text-decoration: none;
     color: black;
     padding: 0 0.5rem;
 }

 nav a:hover {
     text-decoration: underline;
 }

 .router-link-active {
     text-decoration: underline;
 }
</style>
