<template>
	<div
		class="gravatar"
		:style="
			'height: ' +
				size +
				'px; width: ' +
				size +
				'px; background-color: ' +
				bg
		"
	>
		<span v-if="showInitials">{{ initials }}</span>
		<div
			v-if="!showInitials"
			:style="
				'background-image: url(https://www.gravatar.com/avatar/' +
					hash +
					'?s=' +
					size * 4 +
					'&d=blank); height: ' +
					size +
					'px; width: ' +
					size +
					'px'
			"
		></div>
	</div>
</template>

<script>
export default {
	name: 'gravatar',
	components: {},
	data() {
		return {
			showInitials: true,
			retried: 0
		};
	},
	props: {
		email: {
			default: '',
			type: String
		},
		size: {
			default: 128,
			type: Number
		},
		initials: {
			default: '',
			type: String
		}
	},
	computed: {
		hash: function() {
			var md5 = require('md5');
			return md5(this.email);
		},
		bg: function() {
			return [
				'#0288D1',
				'#0097A7',
				'#00796B',
				'#388E3C',
				'#E64A19',
				'#5D4037',
				'#1976D2',
				'#303F9F',
				'#512DA8',
				'#7B1FA2',
				'#C2185B',
				'#D32F2F'
			][Math.floor(Math.random() * Math.floor(12))];
		}
	},
	methods: {
		getImage: function() {
			try {
				var http = new XMLHttpRequest();
				var url =
					'https://www.gravatar.com/avatar/' +
					this.hash +
					'?s=10&d=404';
				http.open('HEAD', url, false);
				http.send();
				this.showInitials = http.status != 200;
			} catch {
				this.showInitials = true;
				if (this.retried < 5) {
					this.retried++;
					setTimeout(this.getImage, 2000);
				}
			}
		}
	},
	mounted() {
		this.getImage();
	}
};
</script>

<style lang="scss" scoped>
.gravatar {
	display: block;
	position: relative;
	user-select: none;
	& > * {
		position: absolute;
		top: 50%;
		left: 50%;
		transform: translate(-50%, -50%);
		z-index: 3;
	}
	span {
		width: max-content;
		text-align: center;
		text-transform: capitalize;
		font-weight: 600;
		color: rgba(white, 0.8);
	}
	& > div {
		background-size: cover;
		background-position: center;
	}
}
</style>
