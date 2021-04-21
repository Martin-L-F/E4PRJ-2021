<template>
	<div class="scoreboard">
		<div class="rings"></div>
		<div class="rings"></div>
		<div class="lines"></div>
		<div class="lines"></div>

		<h3>
			<span class="focus-color">Current score</span>
			<span class="slim"> | John Doe</span>
		</h3>
		<h1>20.000</h1>
	</div>
</template>

<script>
	export default {
		components: {},
	};
</script>

<style lang="scss" scoped>
	.scoreboard {
		h3 {
			margin: 0 0 calc(var(--viewport-size) * -0.025) 0;
			font-size: calc(var(--viewport-size) * 0.025);
			z-index: 4;
			position: relative;
		}
		h1 {
			z-index: 4;
			position: relative;
			text-align: center;
			font-size: calc(var(--viewport-size) * 0.15);
			padding-left: 0.1em;
			margin: 0;
			text-shadow: 0 0 0.3em var(--white-3);
		}
	}
	.rings {
		pointer-events: none;
		position: absolute;
		z-index: 0;
		top: 50%;
		left: 50%;
		transform: translate(-50%, -50%);
		width: calc(var(--viewport-size) * 0.85);
		padding-top: calc(var(--viewport-size) * 0.85);
		border-radius: 100%;
		border: 2px solid var(--white-3);
		box-shadow: 0 0 10px 0px var(--white-3),
			inset 0 0 10px 0px var(--white-1);
		backdrop-filter: blur(5px);
	}
	.rings:nth-child(2) {
		width: calc(var(--viewport-size) * 0.75);
		padding-top: calc(var(--viewport-size) * 0.75);
		backdrop-filter: blur(15px);
	}
	.lines {
		pointer-events: none;
		position: absolute;
		top: 50%;
		left: 50%;
		transform: translate(-50%, -50%) rotate(12deg);
		width: calc(var(--viewport-size) * 0.9);
		height: 0;
		clip-path: polygon(
			0% 0%,
			0% 100%,
			10% 100%,
			10% 0,
			90% 0,
			90% 100%,
			100% 100%,
			100% 0%
		);

		border-top: 2px solid var(--white-3);
		box-shadow: 0 0 10px 0px var(--white-3),
			inset 0 0 10px 0px var(--white-1);
	}
	.lines:nth-child(3) {
		animation: line-rotate 4s -2s infinite reverse,
			line-size 7s -2.5s infinite;
	}
	.lines:nth-child(4) {
		transform: translate(-50%, -50%) rotate(-10deg);
		animation: line-rotate 5s infinite, line-size 7.5s infinite;
	}

	@keyframes line-rotate {
		0% {
			transform: translate(-50%, -50%) rotate(-10deg);
		}
		40% {
			transform: translate(-50%, -50%) rotate(25deg);
		}
		70% {
			transform: translate(-50%, -50%) rotate(-15deg);
		}
		100% {
			transform: translate(-50%, -50%) rotate(-10deg);
		}
	}
	@keyframes line-size {
		0% {
			width: calc(var(--viewport-size) * 0.9);
		}
		40% {
			width: calc(var(--viewport-size) * 0.98);
		}
		70% {
			width: calc(var(--viewport-size) * 0.86);
		}
		100% {
			width: calc(var(--viewport-size) * 0.9);
		}
	}
</style>
