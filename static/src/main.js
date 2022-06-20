import { createApp } from 'vue'

import { library } from '@fortawesome/fontawesome-svg-core'
import { FontAwesomeIcon } from '@fortawesome/vue-fontawesome'
import { faPlay } from '@fortawesome/free-solid-svg-icons'

import App from './App.vue'
import router from './router'


library.add(faPlay)

createApp(App)
    .component('font-awesome-icon', FontAwesomeIcon)
    .use(router)
    .mount("#app")
