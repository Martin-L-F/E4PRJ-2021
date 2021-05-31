import { createStore } from "vuex";

export default createStore({
  state() {
    return {
      users: [],
      pistols: [],
      connected: false,
      isPrimary: true
    }
  },
  mutations: {
    ws_connected(state, isPrimary) {
      state.connected = true;
      state.isPrimary = isPrimary;
    },
    ws_disconnected(state) {
      state.connected = false;
    },
    updateUsers(state, users) {
      state.users = users;
    },
    updatePistols(state, pistols) {
      state.pistols = pistols;
    },
  },
  actions: {},
  getters: {
    getPistols: state => {
      return state.pistols;
    },
    getUsers: state => {
      return state.users;
    },
    getConnected: state => {
      return state.connected;
    },
    getIsPrimary: state => {
      return state.isPrimary;
    }
  },
  modules: {}
});
