<script setup>
 const props = defineProps({
     content: String,
     showing: {
         type: Boolean,
         default: false,
     }
 })

</script>

<template>
    <Transition>
        <div v-show="showing" class="popup-notification">
            {{ content }}
        </div>
    </Transition>
</template>

<style lang="scss" scoped>
 .popup-notification {
     position: absolute;
     left: 50%;
     top: 5%;
     transform: translateX(-50%);
     font-weight: bold;
     color: white;
     background-color: #58aa00;
     padding: 0.8rem;
     border-radius: 5px;
     z-index: 9999;
 }

 .v-enter-active,
 .v-leave-active {
     transition: opacity 0.5s ease;
 }

 .v-enter-from,
 .v-leave-to {
     opacity: 0;
 }

</style>
