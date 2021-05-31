import { createApp } from "vue";
import App from "./App.vue";
import router from "./router";
import store from "./store";
import { initiateSpeech } from "./voice.js";
import {
  connect,
  addConnectedHandler,
  addDisconnectedHandler,
  addUsersUpdated,
  addPistolsUpdated,
  addShowStatsHandler
} from "./ws_client.js";

createApp(App)
  .use(store)
  .use(router)
  .mount("#app");

addConnectedHandler((isPrimary) => {
  store.commit("ws_connected", isPrimary);
  router.push("/");
});

addDisconnectedHandler(() => {
  store.commit("ws_disconnected");
  router.push("/loading");
});

addUsersUpdated(users => {
  store.commit('updateUsers', users);
});

addPistolsUpdated(pistols => {
  store.commit('updatePistols', pistols);
});

addShowStatsHandler(() => {
  router.push("/showstats");
});

if (localStorage.getItem('ControlUnit-IP') == null) {
  localStorage.setItem('ControlUnit-IP', 'localhost');
}

connect('ws://' + localStorage.getItem('ControlUnit-IP') + ':3000');

initiateSpeech();
