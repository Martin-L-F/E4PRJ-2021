<template>
  <div class="bg">
    <div class="spin"></div>
    <div class="spin"></div>
    <div class="spin"></div>
    <div class="spin"></div>
    <div class="spin"></div>
    <div class="spin"></div>
    <div class="spin"></div>
    <div class="spin"></div>
    <div class="spin"></div>
    <div class="spin"></div>
  </div>
  <router-view v-slot="{ Component }">
    <transition name="fade" mode="out-in">
      <component :is="Component" />
    </transition>
  </router-view>
</template>

<style lang="scss">
@import "./src/assets/style/style.scss";
</style>
