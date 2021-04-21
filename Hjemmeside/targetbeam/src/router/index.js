import { createRouter, createWebHashHistory } from "vue-router";
import Home from "../views/Home.vue";

function loginChecker(to, from, next) {
  var isAuthenticated = false;
  if (localStorage.getItem("token") != "" /*store.getters.user != null*/) {
    isAuthenticated = true;
  } else {
    isAuthenticated = false;
  }
  if (isAuthenticated) {
    next(); // allow to enter route
  } else {
    next('/signin'); // go to '/login';
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
    path: "/signin",
    name: "Signin",
    component: () =>
      import("../views/Signin.vue")
  }
];

const router = createRouter({
  history: createWebHashHistory(),
  routes
});

export default router;
