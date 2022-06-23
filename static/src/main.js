import { createApp } from 'vue'

import { library } from '@fortawesome/fontawesome-svg-core'
import { FontAwesomeIcon } from '@fortawesome/vue-fontawesome'
import { faPlay, faClipboard } from '@fortawesome/free-solid-svg-icons'

import App from './App.vue'
import router from './router'


library.add(faPlay)
library.add(faClipboard)

createApp(App)
    .component('font-awesome-icon', FontAwesomeIcon)
    .use(router)
    .mount("#app")
