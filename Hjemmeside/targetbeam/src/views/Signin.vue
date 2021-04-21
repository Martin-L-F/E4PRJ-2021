<template>
	<div class="split">
		<div class="box">
			<h1>TARGET<span class="focus-color">BEAM</span></h1>
			<p class="description">SIGN IN . . .</p>
		</div>
		<div class="box signin">
			<input type="text" :value="visiblePassword" />
			<div class="progress-step">
				<div
					class="step"
					:class="{
						active: attempts > 0 && attempts < 3,
						error: attempts == 3,
					}"
				></div>
				<div
					class="step"
					:class="{
						active: attempts > 1 && attempts < 3,
						error: attempts == 3,
					}"
				></div>
				<div
					class="step"
					:class="{
						active: attempts > 2 && attempts < 3,
						error: attempts == 3,
					}"
				></div>
			</div>
			<div class="numpad">
				<div
					v-for="n in [7, 8, 9, 4, 5, 6, 1, 2, 3, 0]"
					:key="n"
					class="btn numkey"
					:class="{ disabled: !acceptsInput || password.length > 5 }"
					v-on:click="inputNumber(n)"
				>
					{{ n }}
				</div>
				<div
					class="btn numkey"
					:class="{ disabled: !acceptsInput || password.length == 0 }"
					v-on:click="password = ''"
				>
					C
				</div>
				<div
					class="btn numkey"
					:class="{ disabled: !acceptsInput || password.length < 6 }"
					v-on:click="signin()"
				>
					⏎
				</div>
			</div>
		</div>
	</div>
</template>

<script>
	import { speak } from "@/voice.js";
	export default {
		name: "Signin",
		data() {
			return {
				password: "",
				attempts: 0,
				acceptsInput: true,
				cooldowntimer: null,
			};
		},
		methods: {
			inputNumber: function(input) {
				if (this.password.length < 6) {
					this.password += input;
				}
			},
			signin: function() {
				if (this.password.length == 6) {
					if (this.attempts < 3) {
						if (this.password == "123456") {
							localStorage.setItem("token", this.password);
							speak("Welcome to Target Beam");
							this.$router.push("/");
						} else {
							speak("Password incorrect");
							this.attempts += 1;
							if (this.attempts > 2) {
								speak("Initiating secure lockdown");
								this.acceptsInput = false;
								if (this.cooldowntimer != null) {
									clearTimeout(this.cooldowntimer);
								}
								this.cooldowntimer = setTimeout(() => {
									this.attempts = 0;
									this.acceptsInput = true;
								}, 60000);
							}
						}
						this.password = "";
					}
				}
			},
		},
		computed: {
			visiblePassword: function() {
				return this.password + "*".repeat(6 - this.password.length);
			},
		},
		mounted() {
			window.addEventListener("keyup", (ev) => {
				if (
					this.acceptsInput &&
					this.password.length < 6 &&
					ev.key >= "0" &&
					ev.key <= "9"
				) {
					this.inputNumber(ev.key);
				} else if (
					ev.key == "Enter" &&
					this.acceptsInput &&
					this.password.length == 6
				) {
					this.signin();
				}
			});
		},
	};
</script>
