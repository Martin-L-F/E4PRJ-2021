import { createRouter, createWebHashHistory } from "vue-router";
import Home from "../views/Home.vue";
import store from "../store";

function loginChecker(to, from, next) {
  var isAuthenticated = false;
  if (store.getters.getConnected && store.getters.getIsPrimary) {
    isAuthenticated = true;
  } else {
    isAuthenticated = false;
  }

  if (isAuthenticated) {
    next(); // allow to enter route
  } else {
    next("/loading"); // go to '/login';
  }
}

const routes = [
  {
    path: "/",
    name: "Home",
    component: Home,
    beforeEnter: loginChecker
  },
  {
    path: "/scoreboard",
    name: "Scoreboard",
    component: import("../views/Scoreboard.vue"),
    beforeEnter: loginChecker
  },
  {
    path: "/loading",
    name: "Loading",
    component: () => import("../views/Loading.vue")
  },
  {
    path: "/countdown",
    name: "countdown",
    component: () => import("../views/countdown.vue"),
    beforeEnter: loginChecker
  },
  {
    path: "/showstats",
    name: "ShowStats",
    component: () => import("../views/ShowStats.vue"),
    beforeEnter: loginChecker
  }
];

const router = createRouter({
  history: createWebHashHistory(process.env.BASE_URL),
  routes
});

export default router;
