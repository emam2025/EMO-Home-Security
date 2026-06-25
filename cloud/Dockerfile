FROM node:20-alpine AS base
WORKDIR /app
RUN corepack enable && corepack prepare pnpm@9 --activate

FROM base AS development
COPY package.json pnpm-lock.yaml ./
RUN pnpm install
COPY prisma ./prisma
RUN pnpm prisma:generate
COPY . .
EXPOSE 3000
CMD ["pnpm", "start:dev"]

FROM base AS production
COPY package.json pnpm-lock.yaml ./
RUN pnpm install --prod
COPY prisma ./prisma
RUN pnpm prisma:generate
COPY . .
RUN pnpm build
EXPOSE 3000
CMD ["node", "dist/main"]
