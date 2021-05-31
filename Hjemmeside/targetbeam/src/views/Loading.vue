<template>
  <div :class="{ error: !connected }">
    <div class="rings"></div>
    <div class="rings"></div>
    <div class="lines"></div>
    <div class="lines"></div>
    <div class="title">
      <h1>
        TARGET<span class="focus-color" :class="{ error: !connected }"
          >BEAM</span
        >
      </h1>
    </div>
  </div>
</template>

<script>
export default {
  computed: {
    connected() {
      return this.$store.getters.getConnected;
    }
  }
};
</script>

<style lang="scss" scoped>
.title {
  z-index: 2;
  position: relative;
  h1 {
    font-size: calc(var(--viewport-size) * 0.08);
  }
}
.error * {
  animation-play-state: paused;
}
</style>
